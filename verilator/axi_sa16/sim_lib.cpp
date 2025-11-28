// sim_lib.cpp — build as a shared library (e.g., libVtop.so)
//
// 목적: IREE가 호출하는 DPI 심볼에 대해 "타이밍 전용" 오프로딩 모델 제공.
//       - 실제 RTL 연산은 수행하지 않음(결과는 zero-fill)
//       - 연산/통신 오버헤드를 사이클로 모델링하고 wall-time으로 소비(sleep)
//       - 오프로딩 비율/파라미터에 따라 벤치마크 시간이 달라짐

#include <verilated.h>
#include "Vaxi_sa16.h"
#if VM_TRACE
#  include <verilated_vcd_c.h>
#endif

#include <atomic>
#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

// ------------------------------------------------------------------
// 전역(시뮬레이터/상태)
// ------------------------------------------------------------------
static Vaxi_sa16* top = nullptr;
static vluint64_t sim_time = 0;
#if VM_TRACE
static VerilatedVcdC* tfp = nullptr;
#endif

// 결과 버퍼(옵션; 지금은 zero-fill이 기본)
static std::vector<float> s_last_result_buffer;
static int s_last_result_m = 0, s_last_result_n = 0;

// 스레드/상태
static std::thread g_sim_thread;
static std::atomic<bool> g_sim_thread_running{false};
// 상태: 0=idle, 1=running, 2=done (단순화)
static std::atomic<int>  g_job_state{0};
static std::mutex g_job_mutex;
static std::condition_variable g_job_cv;
static bool g_job_started = false;  // CV spurious wakeup 대비

// 사이클 카운터(atomic)
static std::atomic<uint64_t> g_sim_cycles{0};       // 누적 사이클(모델 시점)
static std::atomic<uint64_t> g_job_start_cycles{0}; // 잡 시작 사이클
static std::atomic<uint64_t> g_last_job_cycles{0};  // 마지막 잡 소요 사이클
static std::atomic<uint64_t> g_job_target_cycles{0};
static std::atomic<bool>     g_count_enabled{false};

// ------------------------------------------------------------------
// 파라미터/환경변수
// ------------------------------------------------------------------
// 어레이 타일링(분석 모델)
static uint32_t SA16_A_LANES = 16;   // K 축 타일
static uint32_t SA16_B_LANES = 64;   // N(OC) 축 타일
static uint32_t SA16_WARMUP   = 64;  // k-타일 당 워밍업
static uint32_t SA16_COMPUTE  = 128; // k-타일 당 compute
static uint32_t SA16_SETTLE   = 6;   // k-타일 당 settle
static uint32_t SA16_B_PRELOAD_BEATS = 64; // 가중치 프리로드 대략치

static enum class Sa16Mode { Stub, RTL } SA16_MODE = Sa16Mode::Stub;

// 가짜 가속 비율(0.0~1.0): 1.0일수록 더 큰 단축(최대 90%)
static double SA16_FAKE_RATIO = 0.0;

// 시뮬레이터 클럭(Hz) — 사이클을 실시간으로 변환할 때 사용
static double SA16_FREQ_HZ = 1e9; // 1000 MHz 기본

// DMA/통신 모델(대역폭/지연)
static double SA16_PCIE_GBPS  = 12.0; // 유효 대역폭(GB/s)
static double SA16_PCIE_LAT_US = 10.0; // 트랜잭션 고정 지연(μs)

// Edge Device
// static double SA16_FREQ_HZ = 5e8; // 500 MHz 기본
// static double SA16_PCIE_GBPS  = 2.0; // 유효 대역폭(GB/s)
// static double SA16_PCIE_LAT_US = 30.0; // 트랜잭션 고정 지연(μs)

// 디버그 로그
static bool ACCEL_DEBUG = false;

static void rtl_start_conv(uint32_t N, uint32_t H, uint32_t W, uint32_t C,
                           uint32_t OC, uint32_t KH, uint32_t KW,
                           uint32_t sh, uint32_t sw, uint32_t ph, uint32_t pw);
static bool rtl_is_done();
static void rtl_clear_done();

// ------------------------------------------------------------------
static inline uint64_t ceil_div_u64(uint64_t a, uint64_t b) {
  return (a + b - 1) / b;
}

static inline uint32_t out_dim(uint32_t H, uint32_t K, uint32_t stride, uint32_t pad) {
  return (H + 2*pad - K)/stride + 1;
}

// 가짜 가속 스케일: 10% 바닥 ~ 90%까지 단축
static inline uint64_t scale_cycles_by_ratio(uint64_t cyc) {
  double f = 1.0 - 0.90 * SA16_FAKE_RATIO; // ratio↑ → 더 작은 f
  if (f < 0.10) f = 0.10;
  return (uint64_t)std::ceil((double)cyc * f);
}

static void load_env_overrides() {
  auto posi = [](const char* s, int def, int minv){ if(!s) return def; int v=atoi(s); return v<minv?minv:v; };

  SA16_A_LANES = posi(std::getenv("SA16_A_LANES"), 16, 1);
  SA16_B_LANES = posi(std::getenv("SA16_B_LANES"), 64, 1);
  SA16_WARMUP  = posi(std::getenv("SA16_WARMUP"),  64, 1);
  SA16_COMPUTE = posi(std::getenv("SA16_COMPUTE"),128, 1);
  SA16_SETTLE  = posi(std::getenv("SA16_SETTLE"),   6, 1);

  if (const char* s = std::getenv("SA16_B_PRELOAD_BEATS")) SA16_B_PRELOAD_BEATS = std::max(0, atoi(s));
  if (const char* s = std::getenv("SA16_FAKE_RATIO")) {
    double r = atof(s); SA16_FAKE_RATIO = std::max(0.0, std::min(1.0, r));
  }
  if (const char* s = std::getenv("SA16_FREQ_HZ"))     SA16_FREQ_HZ = std::max(1.0, atof(s));
  if (const char* s = std::getenv("SA16_PCIE_GBPS"))   SA16_PCIE_GBPS = std::max(0.1, atof(s));
  if (const char* s = std::getenv("SA16_PCIE_LAT_US")) SA16_PCIE_LAT_US = std::max(0.0, atof(s));

  if (const char* s = std::getenv("ACCEL_DEBUG"))
    ACCEL_DEBUG = (*s && std::strcmp(s,"0")!=0);

  if (const char* s = std::getenv("SA16_MODE")) {
    if (!std::strcmp(s,"rtl") || !std::strcmp(s,"RTL")) SA16_MODE = Sa16Mode::RTL;
    else SA16_MODE = Sa16Mode::Stub;
  }

}

// DMA 모델: bytes → cycles (대역폭/지연을 클럭으로 환산)
static inline uint64_t dma_cycles_for_bytes(uint64_t bytes) {
  double sec = (bytes / (SA16_PCIE_GBPS * 1e9)) + (SA16_PCIE_LAT_US * 1e-6);
  return (uint64_t)std::ceil(sec * SA16_FREQ_HZ);
}

// GEMM/CONV 분석 모델
static uint64_t model_gemm_cycles(uint32_t M, uint32_t K, uint32_t N) {
  const uint64_t K_tiles = ceil_div_u64((uint64_t)K, SA16_A_LANES);
  const uint64_t N_tiles = ceil_div_u64((uint64_t)N, SA16_B_LANES);
  const uint64_t per_k_tile = (uint64_t)SA16_WARMUP + SA16_COMPUTE + SA16_SETTLE;

  uint64_t cyc = (uint64_t)M * N_tiles * K_tiles * per_k_tile;
  cyc += SA16_B_PRELOAD_BEATS;
  if (ACCEL_DEBUG) {
    std::fprintf(stderr,
      "[SA16][GEMM] M=%u K=%u N=%u | Kt=%" PRIu64 " Nt=%" PRIu64 " per=%" PRIu64 " preload=%u base=%" PRIu64 "\n",
      M,K,N,K_tiles,N_tiles,per_k_tile,SA16_B_PRELOAD_BEATS,cyc);
  }
  return cyc?cyc:1;
}

static uint64_t model_conv_cycles(uint32_t N, uint32_t H, uint32_t W,
                                  uint32_t C, uint32_t OC,
                                  uint32_t KH, uint32_t KW,
                                  uint32_t sh, uint32_t sw,
                                  uint32_t ph, uint32_t pw) {
  const uint32_t OH = out_dim(H, KH, sh, ph);
  const uint32_t OW = out_dim(W, KW, sw, pw);
  const uint64_t M_positions = (uint64_t)N * OH * OW;
  const uint64_t K_total     = (uint64_t)KH * KW * C;
  const uint64_t OC_total    = (uint64_t)OC;
  const uint64_t K_tiles  = ceil_div_u64(K_total, SA16_A_LANES);
  const uint64_t OC_tiles = ceil_div_u64(OC_total, SA16_B_LANES);
  const uint64_t per_k_tile = (uint64_t)SA16_WARMUP + SA16_COMPUTE + SA16_SETTLE;

  uint64_t cyc = M_positions * OC_tiles * K_tiles * per_k_tile;
  cyc += SA16_B_PRELOAD_BEATS;
  if (ACCEL_DEBUG) {
    std::fprintf(stderr,
      "[SA16][CONV] N=%u HxW=%ux%u C=%u OC=%u KHxKW=%ux%u sh/sw=%u/%u ph/pw=%u/%u | "
      "OHxOW=%ux%u Kt=%" PRIu64 " OCt=%" PRIu64 " base=%" PRIu64 "\n",
      N,H,W,C,OC,KH,KW,sh,sw,ph,pw,OH,OW,K_tiles,OC_tiles,cyc);
  }
  return cyc?cyc:1;
}

// ------------------------------------------------------------------
// 트레이스용 tick (지금은 실시간 슬립 모델이므로 필수는 아님)
// ------------------------------------------------------------------
static inline void tick_once() {
#if VM_TRACE
  top->clk = 0; top->eval(); tfp->dump(++sim_time);
  top->clk = 1; top->eval(); tfp->dump(++sim_time);
#else
  top->clk = !top->clk; top->eval();
#endif
}

// ------------------------------------------------------------------
// 시뮬 스레드: target까지 남은 사이클 → 실제 시간으로 슬립
// ------------------------------------------------------------------
static void sim_thread_main() {
  while (g_sim_thread_running.load(std::memory_order_acquire)) {
    if (g_job_state.load(std::memory_order_acquire) == 1) {
      uint64_t target = g_job_target_cycles.load(std::memory_order_acquire);
      uint64_t now    = g_sim_cycles.load(std::memory_order_acquire);
      uint64_t remain = (target > now) ? (target - now) : 0;

      if (remain > 0) {
        // 사이클을 시간으로 환산해 슬립
        double sec = remain / SA16_FREQ_HZ;
        std::this_thread::sleep_for(std::chrono::duration<double>(sec));
        g_sim_cycles.store(target, std::memory_order_release);
      }

      uint64_t dur = g_sim_cycles.load(std::memory_order_acquire)
                   - g_job_start_cycles.load(std::memory_order_acquire);
      g_last_job_cycles.store(dur, std::memory_order_release);
      g_count_enabled.store(false, std::memory_order_release);
      
      // [수정] mutex를 명시적으로 잠궈서 notify 안전성 보장
      {
        std::unique_lock<std::mutex> lk(g_job_mutex);
        g_job_state.store(2, std::memory_order_release);
      }
      g_job_cv.notify_all();

      if (ACCEL_DEBUG) {
        std::fprintf(stderr, "[SIM] job completed cycles=%" PRIu64 " (%.3f ms @ %.0f MHz)\n",
                     dur, 1e3 * (double)dur / SA16_FREQ_HZ, SA16_FREQ_HZ/1e6);
      }
    } else {
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  }
}

// ===== Export visibility macro =====
#if defined(_WIN32) || defined(_WIN64)
  #define SA16_EXPORT __declspec(dllexport)
  #define SA16_USED
#else
  #define SA16_EXPORT __attribute__((visibility("default")))
  #define SA16_USED   __attribute__((used))
#endif

extern "C" {
// --- forward declarations for symbols used before their definitions ---
SA16_EXPORT SA16_USED int  sa16_conv_offload(
  const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
  uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
  uint32_t KH, uint32_t KW, uint32_t sh, uint32_t sw, uint32_t ph, uint32_t pw);

SA16_EXPORT SA16_USED void sa16_conv_wait_for_completion(
  uint8_t* output, uint32_t N, uint32_t OH, uint32_t OW, uint32_t OC);

SA16_EXPORT SA16_USED void sa16_matmul_bias_add_offload(
  const uint8_t* lhs_ptr, const uint8_t* rhs_ptr, const uint8_t* bias_ptr,
  uint32_t M, uint32_t K, uint32_t N);

// (필요 시) iree_dpi_*와 *_relu_offload 도 여기서 미리 선언 가능
}

// ------------------------------------------------------------------
// 공개 타이밍 쿼리
// ------------------------------------------------------------------
extern "C" SA16_EXPORT uint64_t sa16_get_cycles(void) {
  int s = g_job_state.load(std::memory_order_acquire);
  if (s == 1) return g_sim_cycles.load() - g_job_start_cycles.load();
  if (s == 2) return g_last_job_cycles.load();
  return g_last_job_cycles.load();
}
extern "C" SA16_EXPORT int sa16_is_busy(void) {
  return g_job_state.load(std::memory_order_acquire) == 1;
}
extern "C" SA16_EXPORT uint64_t sa16_get_cycles_live(void) {
  if (sa16_is_busy()) return g_sim_cycles.load() - g_job_start_cycles.load();
  return g_last_job_cycles.load();
}
extern "C" SA16_EXPORT uint64_t sa16_get_cycles_latched(void) {
  int s = g_job_state.load(std::memory_order_acquire);
  if (s == 2 || s == 0) return g_last_job_cycles.load();
  return 0;
}

// ------------------------------------------------------------------
// 모델 기반 측정 API: dma latency/bandwidth 및 모델화된 연산 시간
// 이 함수들은 현재 시뮬레이터의 DMA 모델(dma_cycles_for_bytes)과
// 클럭 주파수(SA16_FREQ_HZ)를 사용해 시간/대역폭을 계산해 반환합니다.
// 실시간 측정(사이클 카운터) 대신 모델 기반 예측을 제공합니다.
// ------------------------------------------------------------------

extern "C" SA16_EXPORT void sa16_dma_model_stats(
    uint64_t size1_bytes, uint64_t size2_bytes,
    double* out_latency_s, double* out_bw_bytes_per_s) {
  if (!out_latency_s || !out_bw_bytes_per_s) return;
  if (size1_bytes == 0 || size2_bytes == 0 || size1_bytes == size2_bytes) {
    // fallback: compute single-point values
    uint64_t c = dma_cycles_for_bytes(size1_bytes ? size1_bytes : 1);
    double t = (double)c / SA16_FREQ_HZ;
    *out_latency_s = SA16_PCIE_LAT_US * 1e-6;
    *out_bw_bytes_per_s = (size1_bytes>0) ? (double)size1_bytes / t : SA16_PCIE_GBPS * 1e9;
    return;
  }

  uint64_t c1 = dma_cycles_for_bytes(size1_bytes);
  uint64_t c2 = dma_cycles_for_bytes(size2_bytes);
  double t1 = (double)c1 / SA16_FREQ_HZ;
  double t2 = (double)c2 / SA16_FREQ_HZ;

  // linear model: t = latency + size / bw  => slope = (t2-t1)/(s2-s1) = 1/bw
  double slope = (t2 - t1) / ((double)size2_bytes - (double)size1_bytes);
  if (slope <= 0) slope = 1.0 / (SA16_PCIE_GBPS * 1e9);
  double bw = 1.0 / slope;
  double latency = t1 - slope * (double)size1_bytes;
  if (latency < 0) latency = SA16_PCIE_LAT_US * 1e-6;

  *out_latency_s = latency;
  *out_bw_bytes_per_s = bw;
}

extern "C" SA16_EXPORT double sa16_dma_time_for_bytes(uint64_t bytes) {
  uint64_t c = dma_cycles_for_bytes(bytes);
  return (double)c / SA16_FREQ_HZ;
}

// GEMM/CONV 모델 기반 시간 및 유효 pAcc 계산
extern "C" SA16_EXPORT double sa16_gemm_model_time(uint32_t M, uint32_t K, uint32_t N) {
  uint64_t base = model_gemm_cycles(M,K,N);
  uint64_t cyc = scale_cycles_by_ratio(base);
  return (double)cyc / SA16_FREQ_HZ;
}

extern "C" SA16_EXPORT double sa16_gemm_model_pacc(uint32_t M, uint32_t K, uint32_t N) {
  // FLOPs for GEMM (multiply-add counted as 2 FLOP per MAC)
  double flops = 2.0 * (double)M * (double)K * (double)N;
  double t = sa16_gemm_model_time(M,K,N);
  if (t <= 0.0) return 0.0;
  return flops / t;
}

extern "C" SA16_EXPORT double sa16_conv_model_time(
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t sh, uint32_t sw, uint32_t ph, uint32_t pw) {
  uint64_t base = model_conv_cycles(N,H,W,C,OC,KH,KW,sh,sw,ph,pw);
  uint64_t cyc = scale_cycles_by_ratio(base);
  return (double)cyc / SA16_FREQ_HZ;
}

extern "C" SA16_EXPORT double sa16_conv_model_pacc(
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t sh, uint32_t sw, uint32_t ph, uint32_t pw) {
  // Estimate FLOPs: positions * OC * KH * KW * C * 2
  uint32_t OH = out_dim(H, KH, sh, ph);
  uint32_t OW = out_dim(W, KW, sw, pw);
  double positions = (double)N * (double)OH * (double)OW;
  double kernel_elems = (double)KH * (double)KW * (double)C;
  double flops = positions * (double)OC * kernel_elems * 2.0;
  double t = sa16_conv_model_time(N,H,W,C,OC,KH,KW,sh,sw,ph,pw);
  if (t <= 0.0) return 0.0;
  return flops / t;
}


// ------------------------------------------------------------------
// 초기화/종료
// ------------------------------------------------------------------
extern "C" SA16_EXPORT int sa16_init(void) {
  if (top) return 0;
#if VM_TRACE
  Verilated::traceEverOn(true);
#endif
  load_env_overrides();

  top = new Vaxi_sa16;
#if VM_TRACE
  tfp = new VerilatedVcdC;
  top->trace(tfp, 99);
  tfp->open("sim.vcd");
#endif
  top->clk = 0; top->rst = 1; top->eval();
  for (int i=0;i<4;i++) tick_once();
  top->rst = 0; top->eval();

  g_sim_cycles.store(0);
  g_job_state.store(0);
  g_last_job_cycles.store(0);
  g_job_target_cycles.store(0);
  g_job_start_cycles.store(0);
  g_count_enabled.store(false);

  g_sim_thread_running.store(SA16_MODE == Sa16Mode::Stub);  
  if (SA16_MODE == Sa16Mode::Stub) g_sim_thread = std::thread(sim_thread_main);

  std::fprintf(stderr,
    "[SA16] lanes(A=%u,B=%u) warmup=%u compute=%u settle=%u preload=%u\n"
    "[SA16] fake_ratio=%.2f, freq=%.0f MHz, DMA=%.1f GB/s + %.1f us\n",
    SA16_A_LANES, SA16_B_LANES, SA16_WARMUP, SA16_COMPUTE, SA16_SETTLE, SA16_B_PRELOAD_BEATS,
    SA16_FAKE_RATIO, SA16_FREQ_HZ/1e6, SA16_PCIE_GBPS, SA16_PCIE_LAT_US);

  return 0;
}

extern "C" SA16_EXPORT void sa16_deinit(void) {
  if (!top) return;
  g_sim_thread_running.store(false);
  if (g_sim_thread.joinable()) g_sim_thread.join();
#if VM_TRACE
  if (tfp) { tfp->close(); delete tfp; tfp=nullptr; }
#endif
  top->final();
  delete top; top = nullptr;
  std::fprintf(stderr,"[SIM] sa16_deinit done\n");
}

// ------------------------------------------------------------------
// 오프로딩(타이밍 전용)
// ------------------------------------------------------------------

static inline void start_job_with_cycles(uint64_t expected_cycles, const char* tag) {
  // Mutex로 보호된 안전한 job 시작
  std::unique_lock<std::mutex> lk(g_job_mutex);
  
  // 이전 job이 완료될 때까지 대기 (state != 1)
  while (g_job_state.load(std::memory_order_relaxed) == 1) {
    g_job_cv.wait(lk);
  }

  if (ACCEL_DEBUG) {
    fprintf(stderr, "[SA16] %s: Starting job with %" PRIu64 " cycles\n", tag, expected_cycles);
  }

  if (expected_cycles == 0) expected_cycles = 1;

  uint64_t now = g_sim_cycles.load(std::memory_order_acquire);
  g_job_start_cycles.store(now, std::memory_order_release);
  g_job_target_cycles.store(now + expected_cycles, std::memory_order_release);
  g_last_job_cycles.store(0, std::memory_order_release);
  g_count_enabled.store(true, std::memory_order_release);
  g_job_started = true;  // flag for spurious wakeup
  g_job_state.store(1, std::memory_order_release);
  
  lk.unlock();  // mutex 명시적 해제

  if (ACCEL_DEBUG) {
    std::fprintf(stderr, "[SA16] %s job started, target=%.3f ms\n",
                 tag, 1e3 * (double)expected_cycles / SA16_FREQ_HZ);
  }
}

// GEMM
extern "C" SA16_EXPORT void sa16_matmul_bias_add_offload(
    const uint8_t* lhs_ptr, const uint8_t* rhs_ptr, const uint8_t* bias_ptr,
    uint32_t M, uint32_t K, uint32_t N) {
  if (!top) sa16_init();

  // 계산 사이클
  uint64_t base_cycles = model_gemm_cycles(M, K, N);
  uint64_t comp_cycles = scale_cycles_by_ratio(base_cycles);

  // DMA(가정: fp32)
  const uint64_t A_bytes = (uint64_t)M * K * 4;
  const uint64_t B_bytes = (uint64_t)K * N * 4;
  const uint64_t Bias_bytes = (uint64_t)N * 4;
  const uint64_t C_bytes = (uint64_t)M * N * 4;

  uint64_t dma_cycles = 0;
  dma_cycles += dma_cycles_for_bytes(A_bytes);
  dma_cycles += dma_cycles_for_bytes(B_bytes);
  dma_cycles += dma_cycles_for_bytes(Bias_bytes);
  dma_cycles += dma_cycles_for_bytes(C_bytes);

  uint64_t total = comp_cycles + dma_cycles;

  if (ACCEL_DEBUG) {
    std::fprintf(stderr, "[SA16][GEMM] base=%" PRIu64 " ->comp=%" PRIu64 " + dma=%" PRIu64 " = total=%" PRIu64 "\n",
                 base_cycles, comp_cycles, dma_cycles, total);
  }
  s_last_result_buffer.clear();
  s_last_result_m = s_last_result_n = 0;

  start_job_with_cycles(total, "gemm");
}

// ===== Optional RELU variants (suppress "symbol not found" warnings) =====
extern "C" SA16_EXPORT void sa16_matmul_bias_add_relu_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N)
{
  // 타이밍 전용: RELU 유무와 무관하게 동일한 비용으로 처리
  sa16_matmul_bias_add_offload(lhs, rhs, bias, M, K, N);
}

extern "C" SA16_EXPORT void sa16_conv_relu_offload(
    const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t stride_h, uint32_t stride_w,
    uint32_t pad_h, uint32_t pad_w)
{
  (void)sa16_conv_offload(input, filter, bias,
                          N, H, W, C, OC, KH, KW,
                          stride_h, stride_w, pad_h, pad_w);
}

// (일부 로더가 iree_dpi_* 접두사도 찾는 경우를 대비한 래퍼)
extern "C" SA16_EXPORT void iree_dpi_sa16_matmul_bias_add_relu_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N)
{
  sa16_matmul_bias_add_relu_offload(lhs, rhs, bias, M, K, N);
}

extern "C" SA16_EXPORT void iree_dpi_sa16_conv_relu_offload(
    const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t stride_h, uint32_t stride_w,
    uint32_t pad_h, uint32_t pad_w)
{
  sa16_conv_relu_offload(input, filter, bias,
                         N, H, W, C, OC, KH, KW,
                         stride_h, stride_w, pad_h, pad_w);
}

// CONV
// === 이 블록으로 "정의부"를 통째로 교체 ===
extern "C" SA16_EXPORT int sa16_conv_offload(
    const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t sh, uint32_t sw, uint32_t ph, uint32_t pw) {
  if (!top) sa16_init();

  if (SA16_MODE == Sa16Mode::Stub) {
    // === 기존 STUB 경로 ===
    const uint64_t base_cycles = model_conv_cycles(N,H,W,C,OC,KH,KW,sh,sw,ph,pw);
    const uint64_t comp_cycles = scale_cycles_by_ratio(base_cycles);

    const uint32_t OH = out_dim(H, KH, sh, ph);
    const uint32_t OW = out_dim(W, KW, sw, pw);

    // 가벼운 DMA 비용(가정: fp32)
    const uint64_t In_bytes   = (uint64_t)N * H * W  * C  * 4;
    const uint64_t Flt_bytes  = (uint64_t)KH * KW * C * OC * 4;
    const uint64_t Bias_bytes = (uint64_t)OC * 4;
    const uint64_t Out_bytes  = (uint64_t)N * OH * OW * OC * 4;

    uint64_t dma_cycles = 0;
    dma_cycles += dma_cycles_for_bytes(In_bytes);
    dma_cycles += dma_cycles_for_bytes(Flt_bytes);
    dma_cycles += dma_cycles_for_bytes(Bias_bytes);
    dma_cycles += dma_cycles_for_bytes(Out_bytes);

    start_job_with_cycles(comp_cycles + dma_cycles, "conv2d");
    return 1;
  } else {
    // === RTL 경로 ===
    // busy → idle 보장
    for (;;) {
      int s = g_job_state.load(std::memory_order_acquire);
      if (s == 1) { std::this_thread::yield(); continue; }
      if (g_job_state.compare_exchange_weak(s, 1, std::memory_order_acq_rel)) break;
    }
    g_last_job_cycles.store(0, std::memory_order_release);
    g_count_enabled.store(true, std::memory_order_release);
    g_job_start_cycles.store(g_sim_cycles.load(std::memory_order_acquire), std::memory_order_release);

    // 파라미터 쓰고 start
    rtl_start_conv(N,H,W,C,OC,KH,KW,sh,sw,ph,pw);

    // 비동기: 여기서는 즉시 반환, wait에서 done까지 tick
    return 1;
  }
}

extern "C" SA16_EXPORT void sa16_conv_wait_for_completion(uint8_t* output,
    uint32_t N, uint32_t OH, uint32_t OW, uint32_t OC) {
  if (SA16_MODE == Sa16Mode::Stub) {
    // job 완료 대기
    std::unique_lock<std::mutex> lk(g_job_mutex);
    while (g_job_state.load(std::memory_order_relaxed) != 2) {
      g_job_cv.wait(lk);
    }
    
    // 결과 zero-fill
    if (output) std::memset(output, 0, (size_t)N*OH*OW*OC*sizeof(float));
    
    // 상태를 idle로 설정
    g_job_state.store(0, std::memory_order_release);
    g_job_started = false;
    
    lk.unlock();
    g_job_cv.notify_all();  // 다음 offload 깨우기
    
    return;
  } else {
    // === RTL 경로 ===
    uint64_t cyc0 = g_sim_cycles.load();
    while (!rtl_is_done()) tick_once();
    rtl_clear_done();
    uint64_t cyc1 = g_sim_cycles.load();
    g_last_job_cycles.store(cyc1 - cyc0);
    g_job_state.store(0); g_count_enabled.store(false);

    g_job_cv.notify_all();

    if (output) std::memset(output, 0, (size_t)N*OH*OW*OC*sizeof(float));
    return;
  }
}

extern "C" SA16_EXPORT void sa16_wait_for_completion(uint8_t* out, uint32_t M, uint32_t N) {
  if (ACCEL_DEBUG) {
    fprintf(stderr, "[SA16] sa16_wait_for_completion (M=%u) called, state=%d\n", M, g_job_state.load(std::memory_order_relaxed));
  }

  std::unique_lock<std::mutex> lk(g_job_mutex);
  
  // job 완료 대기 (state == 2)
  while (g_job_state.load(std::memory_order_relaxed) != 2) {
    g_job_cv.wait(lk);
  }
  
  // 결과 zero-fill
  const size_t bytes = (size_t)M * N * sizeof(float);
  if (out) std::memset(out, 0, bytes);
  
  // 상태를 idle로 설정
  g_job_state.store(0, std::memory_order_release);
  g_job_started = false;
  g_count_enabled.store(false, std::memory_order_release);
  
  lk.unlock();
  g_job_cv.notify_all();  // 다음 offload 깨우기

  if (ACCEL_DEBUG) {
    fprintf(stderr, "[SA16] sa16_wait_for_completion (M=%u) returning\n", M);
  }
}

// ------------------------------------------------------------------
// IREE가 찾는 래퍼 심볼(그냥 내부 구현으로 포워딩)
// ------------------------------------------------------------------
extern "C" SA16_EXPORT void iree_dpi_sa16_matmul_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N) {
  sa16_matmul_bias_add_offload(lhs, rhs, bias, M, K, N);
}

extern "C" SA16_EXPORT void iree_dpi_sa16_conv_offload(
    const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t stride_h, uint32_t stride_w,
    uint32_t pad_h, uint32_t pad_w) {
  (void)sa16_conv_offload(input, filter, bias,
                          N,H,W,C,OC,KH,KW,
                          stride_h, stride_w, pad_h, pad_w);
}

extern "C" SA16_EXPORT void iree_dpi_sa16_conv_wait_for_completion(
    uint8_t* output, uint32_t N, uint32_t OH, uint32_t OW, uint32_t OC) {
  sa16_conv_wait_for_completion(output, N, OH, OW, OC);
}

// Connect RTL
static void axi_write(uint32_t addr, uint32_t data) {
  top->s_axi_awvalid = 1; top->s_axi_awaddr = addr;
  top->s_axi_wvalid  = 1; top->s_axi_wdata  = data; top->s_axi_wstrb = 0xF;
  do { tick_once(); } while(!(top->s_axi_awready && top->s_axi_wready));
  top->s_axi_awvalid = 0; top->s_axi_wvalid = 0;
  top->s_axi_bready = 1; tick_once(); top->s_axi_bready = 0;
}

static void axi_read(uint32_t addr, uint32_t* out) {
  top->s_axi_arvalid = 1; top->s_axi_araddr = addr;
  do { tick_once(); } while(!top->s_axi_arready);
  top->s_axi_arvalid = 0;
  top->s_axi_rready = 1; do { tick_once(); } while(!top->s_axi_rvalid);
  *out = top->s_axi_rdata;
  top->s_axi_rready = 0; tick_once();
}

static void rtl_start_conv(uint32_t N, uint32_t H, uint32_t W, uint32_t C,
                           uint32_t OC, uint32_t KH, uint32_t KW,
                           uint32_t sh, uint32_t sw, uint32_t ph, uint32_t pw) {
  axi_write(0x00, N); axi_write(0x04, H); axi_write(0x08, W); axi_write(0x0C, C);
  axi_write(0x10, OC); axi_write(0x14, KH); axi_write(0x18, KW);
  axi_write(0x1C, sh); axi_write(0x20, sw); axi_write(0x24, ph); axi_write(0x28, pw);
  axi_write(0x2C, 1); // start
}

static bool rtl_is_done() {
  uint32_t s = 0; axi_read(0x30, &s); // status: bit1=done 가정
  return (s & 0x2) != 0;
}

static void rtl_clear_done() {
  axi_write(0x34, 1); // done clear
}
