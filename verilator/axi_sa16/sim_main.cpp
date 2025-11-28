//
// Copyright (c) 2010-2021 Antmicro
//
//  This file is licensed under the MIT License.
//  Full license text is available in 'LICENSE' file.
//

#include <verilated.h>
#include "Vaxi_sa16.h"
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#if VM_TRACE
# include <verilated_vcd_c.h>
#endif
#include "src/buses/axi.h"
#include "src/buses/axi-slave.h"
#include "src/buses/axilite.h"
#include "src/renode_bus.h"
#include "src/renode.h"

#include <cstring>   // memcpy
#include <vector>
#include <time.h>    // nanosleep
#include <stdarg.h>

//=========================
// Globals
//=========================
RenodeAgent *axi_sa16 = nullptr;
Vaxi_sa16 *top = new Vaxi_sa16;
VerilatedVcdC *tfp = nullptr;
vluint64_t main_time = 0;

// Track which variants are running so we can post-process results on the host.
static bool g_pending_matmul_relu = false;
static bool g_pending_conv_relu = false;

//=========================
// Debug (always-on)
//=========================
#define DBG(fmt, ...)   do { fprintf(stderr, "[SA16][D] [cyc=%llu] %s: " fmt "\n", \
                                (unsigned long long)main_time, __FUNCTION__, ##__VA_ARGS__); } while(0)
#define DBGV(fmt, ...)  do { fprintf(stderr, "[SA16][V] [cyc=%llu] %s: " fmt "\n", \
                                (unsigned long long)main_time, __FUNCTION__, ##__VA_ARGS__); } while(0)
#define DBG_E(fmt, ...) do { fprintf(stderr, "[SA16][E] [cyc=%llu] %s: " fmt "\n", \
                                (unsigned long long)main_time, __FUNCTION__, ##__VA_ARGS__); } while(0)

static inline void dump_words_sample(const char* tag, const uint32_t* buf, size_t words, size_t head = 2, size_t tail = 2) {
    fprintf(stderr, "[SA16][V] %s: words=%zu\n", tag, words);
    size_t i = 0;
    for (; i < words && i < head; ++i) fprintf(stderr, "  [%zu]=0x%08x\n", i, buf[i]);
    if (words > head + tail) fprintf(stderr, "  ...\n");
    for (size_t t = (words > tail ? words - tail : head); t < words; ++t) {
        if (t < head) continue;
        fprintf(stderr, "  [%zu]=0x%08x\n", t, buf[t]);
    }
}

//=========================
// Eval helper
//=========================
void eval() {
    main_time++;
#if VM_TRACE
    tfp->dump(main_time);
    tfp->flush();
#endif
    top->eval();
}

//=========================
// Agent init
//=========================
RenodeAgent *Init() {
    DBG("Init begin");

    Axi* bus = new Axi(32, 32);

    //=================================================
    // Init bus signals
    //=================================================
    bus->aclk   = &top->clk;
    bus->aresetn= &top->rst; // aresetn(active-low)로 연결

    bus->awid   = &top->s_axi_awid;
    bus->awaddr = (uint32_t *) &top->s_axi_awaddr;
    bus->awlen  = &top->s_axi_awlen;
    bus->awsize = &top->s_axi_awsize;
    bus->awburst= &top->s_axi_awburst;
    bus->awlock = &top->s_axi_awlock;
    bus->awcache= &top->s_axi_awcache;
    bus->awprot = &top->s_axi_awprot;
    bus->awvalid= &top->s_axi_awvalid;
    bus->awready= &top->s_axi_awready;

    bus->wdata  = &top->s_axi_wdata;
    bus->wstrb  = &top->s_axi_wstrb;
    bus->wlast  = &top->s_axi_wlast;
    bus->wvalid = &top->s_axi_wvalid;
    bus->wready = &top->s_axi_wready;

    bus->bid    = &top->s_axi_bid;
    bus->bresp  = &top->s_axi_bresp;
    bus->bvalid = &top->s_axi_bvalid;
    bus->bready = &top->s_axi_bready;

    bus->arid   = &top->s_axi_arid;
    bus->araddr = (uint32_t *) &top->s_axi_araddr;
    bus->arlen  = &top->s_axi_arlen;
    bus->arsize = &top->s_axi_arsize;
    bus->arburst= &top->s_axi_arburst;
    bus->arlock = &top->s_axi_arlock;
    bus->arcache= &top->s_axi_arcache;
    bus->arprot = &top->s_axi_arprot;
    bus->arvalid= &top->s_axi_arvalid;
    bus->arready= &top->s_axi_arready;

    bus->rid    = &top->s_axi_rid;
    bus->rdata  = &top->s_axi_rdata;
    bus->rresp  = &top->s_axi_rresp;
    bus->rlast  = &top->s_axi_rlast;
    bus->rvalid = &top->s_axi_rvalid;
    bus->rready = &top->s_axi_rready;

    bus->evaluateModel = &eval;

    //=================================================
    // Init peripheral
    //=================================================
    axi_sa16 = new RenodeAgent(bus);
    bus->setAgent(axi_sa16);

    DBG("Init done");
    return axi_sa16;
}

//=========================
// Register map (FIXME: 실제 RTL에 맞춰 조정)
//=========================
#define SA16_REG_BASE         0x40000000u   // FIXME: base addr
#define SA16_REG_CONTROL      (SA16_REG_BASE + 0x00)
#define SA16_REG_STATUS       (SA16_REG_BASE + 0x04)
#define SA16_REG_LHS_ADDR     (SA16_REG_BASE + 0x10)
#define SA16_REG_RHS_ADDR     (SA16_REG_BASE + 0x14)
#define SA16_REG_BIAS_ADDR    (SA16_REG_BASE + 0x18)
#define SA16_REG_OUT_ADDR     (SA16_REG_BASE + 0x1C)
#define SA16_REG_DIM_M        (SA16_REG_BASE + 0x20)
#define SA16_REG_DIM_K        (SA16_REG_BASE + 0x24)
#define SA16_REG_DIM_N        (SA16_REG_BASE + 0x28)

#define SA16_CONTROL_START_BIT   (1u << 0)
#define SA16_CONTROL_CLEAR_DONE  (1u << 1)
#define SA16_STATUS_DONE_BIT     (1u << 0)
#define SA16_STATUS_ERROR_BIT    (1u << 1)

#define SA16_MEM_BASE        0x80000000u
#define SA16_MEM_LHS         (SA16_MEM_BASE + 0x00000)
#define SA16_MEM_RHS         (SA16_MEM_BASE + 0x10000)
#define SA16_MEM_BIAS        (SA16_MEM_BASE + 0x20000)
#define SA16_MEM_OUT         (SA16_MEM_BASE + 0x30000)

//=========================
// C API: cycles
//=========================
extern "C" uint64_t sa16_get_cycles() {
    return main_time;
}

extern "C" uint64_t sa16_get_cycles_latched() {
    return sa16_get_cycles();
}

extern "C" uint64_t sa16_get_cycles_live() {
    return sa16_get_cycles();
}

//=========================
// Tick helper
//=========================
static inline void tick_cycles(int steps) {
    if (axi_sa16) axi_sa16->tick(true, steps);
}

//=========================
// Block memory helpers
//=========================
static inline void write_mem_block(uint32_t start_addr, const uint8_t* data, uint32_t size_in_bytes) {
    if (!axi_sa16) { DBG_E("write_mem_block called before init"); return; }
    DBGV("WRITE MEM start=0x%08x size=%u", start_addr, size_in_bytes);

    uint32_t i = 0;
    for (; i + 4 <= size_in_bytes; i += 4) {
        uint32_t word = 0;
        memcpy(&word, data + i, 4);
        axi_sa16->writeToBus(4, start_addr + i, word);
    }
    if (i < size_in_bytes) {
        uint32_t word = 0;
        memcpy(&word, data + i, size_in_bytes - i);
        axi_sa16->writeToBus(4, start_addr + i, word);
    }
}

static inline void read_mem_block(uint8_t* data, uint32_t start_addr, uint32_t size_in_bytes) {
    if (!axi_sa16) { DBG_E("read_mem_block called before init"); return; }
    DBGV("READ  MEM start=0x%08x size=%u", start_addr, size_in_bytes);

    uint32_t i = 0;
    for (; i + 4 <= size_in_bytes; i += 4) {
        axi_sa16->readFromBus(4, start_addr + i);
        Protocol* response = axi_sa16->receive();
        if (response->actionId != readRequest) {
            DBG_E("Invalid response during block read");
            delete response;
            return;
        }
        uint32_t word = response->value;
        delete response;
        memcpy(data + i, &word, 4);
    }
    if (i < size_in_bytes) {
        axi_sa16->readFromBus(4, start_addr + i);
        Protocol* response = axi_sa16->receive();
        if (response->actionId != readRequest) {
            DBG_E("Invalid response during tail read");
            delete response;
            return;
        }
        uint32_t word = response->value;
        delete response;
        memcpy(data + i, &word, size_in_bytes - i);
    }
}

//=========================
// Register helpers
//=========================
static inline uint32_t reg_read(uint32_t addr) {
    if (!axi_sa16) { DBG_E("reg_read before init"); return 0; }
    axi_sa16->readFromBus(4, addr);
    Protocol* response = axi_sa16->receive();
    if (response->actionId != readRequest) {
        DBG_E("Invalid response from RenodeAgent on read addr=0x%08x", addr);
        delete response;
        return 0;
    }
    uint32_t value = response->value;
    delete response;
    DBGV("REG RD  [0x%08x] -> 0x%08x", addr, value);
    return value;
}

static inline void reg_write(uint32_t addr, uint32_t data) {
    if (!axi_sa16) { DBG_E("reg_write before init"); return; }
    DBGV("REG WR  [0x%08x] <- 0x%08x", addr, data);
    axi_sa16->writeToBus(4, addr, data);
}

//=========================
// Init / Deinit
//=========================
extern "C" int sa16_init() {
    DBG("sa16_init called");

    if (!axi_sa16) {
        if (!top) top = new Vaxi_sa16;
        axi_sa16 = Init();
        if (!axi_sa16) {
            DBG_E("Init failed");
            return -1;
        }
    }

    // reset sequence (aresetn active-low)
    top->rst = 0;
    DBG("Reset assert");
    for (int i = 0; i < 20; ++i) tick_cycles(1);

    top->rst = 1;
    DBG("Reset deassert");
    for (int i = 0; i < 20; ++i) tick_cycles(1);

    DBG("sa16_init done");
    return 0;
}

extern "C" void sa16_deinit() {
    DBG("sa16_deinit");
    if (axi_sa16) {
        delete axi_sa16;
        axi_sa16 = nullptr;
    }
    if (top) {
        delete top;
        top = nullptr;
    }
}

//=========================
// MatMul + Bias (RTL path)
//=========================
static void sa16_matmul_issue(const uint8_t* lhs, const uint8_t* rhs,
                              const uint8_t* bias, uint32_t M, uint32_t K,
                              uint32_t N, bool apply_relu) {
    if (!axi_sa16) { DBG_E("SA16 not initialized!"); return; }

    const uint32_t lhs_size  = M * K * 4;
    const uint32_t rhs_size  = K * N * 4;
    const uint32_t bias_size = N * 4;

    DBG("MATMUL issue M=%u K=%u N=%u relu=%d", M, K, N, (int)apply_relu);

    // 1) inputs
    write_mem_block(SA16_MEM_LHS,  lhs,  lhs_size);
    write_mem_block(SA16_MEM_RHS,  rhs,  rhs_size);
    if (bias) write_mem_block(SA16_MEM_BIAS, bias, bias_size);

    // 2) params
    reg_write(SA16_REG_DIM_M, M);
    reg_write(SA16_REG_DIM_K, K);
    reg_write(SA16_REG_DIM_N, N);

    reg_write(SA16_REG_LHS_ADDR,  SA16_MEM_LHS);
    reg_write(SA16_REG_RHS_ADDR,  SA16_MEM_RHS);
    reg_write(SA16_REG_BIAS_ADDR, bias ? SA16_MEM_BIAS : 0);
    reg_write(SA16_REG_OUT_ADDR,  SA16_MEM_OUT);

    // 3) start
    uint32_t ctrl = 0;
    reg_write(SA16_REG_CONTROL, ctrl);
    tick_cycles(1);

    ctrl |= SA16_CONTROL_START_BIT;
    reg_write(SA16_REG_CONTROL, ctrl);
    tick_cycles(2);

    g_pending_matmul_relu = apply_relu;
    DBG("MATMUL started");
}

extern "C" void sa16_matmul_bias_add_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N) {
    sa16_matmul_issue(lhs, rhs, bias, M, K, N, /*apply_relu=*/false);
}

extern "C" void sa16_matmul_bias_add_relu_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N) {
    sa16_matmul_issue(lhs, rhs, bias, M, K, N, /*apply_relu=*/true);
}

extern "C" void sa16_wait_for_completion(uint8_t* out, uint32_t M, uint32_t N) {
    if (!axi_sa16) { DBG_E("SA16 not initialized!"); return; }

    const uint32_t out_size = M * N * 4;
    DBG("WAIT begin (MATMUL) expect_out=%u bytes", out_size);

    while (true) {
        uint32_t status = reg_read(SA16_REG_STATUS);
        if (status & SA16_STATUS_DONE_BIT) break;
        tick_cycles(100);
    }

    read_mem_block(out, SA16_MEM_OUT, out_size);

    if (g_pending_matmul_relu && out) {
        float* out_f32 = (float*)out;
        uint32_t element_count = M * N;
        for (uint32_t i = 0; i < element_count; ++i) {
            if (out_f32[i] < 0.0f) out_f32[i] = 0.0f;
        }
    }
    g_pending_matmul_relu = false;

    DBG("WAIT done (MATMUL)");
}

//=========================
// Convolution (example RTL path)
//=========================
#define SA16_REG_CONV_BASE      (SA16_REG_BASE + 0x100)
#define SA16_REG_CONV_IN_ADDR   (SA16_REG_CONV_BASE + 0x00)
#define SA16_REG_CONV_FIL_ADDR  (SA16_REG_CONV_BASE + 0x04)
#define SA16_REG_CONV_BIAS_ADDR (SA16_REG_CONV_BASE + 0x08)
#define SA16_REG_CONV_OUT_ADDR  (SA16_REG_CONV_BASE + 0x0C)
#define SA16_REG_CONV_NHWC      (SA16_REG_CONV_BASE + 0x10) // [N(8) H(8) W(8) C(8)] example packing
#define SA16_REG_CONV_OC_KW_KH  (SA16_REG_CONV_BASE + 0x14) // [OC(12) KW(10) KH(10)]
#define SA16_REG_CONV_STRIDE    (SA16_REG_CONV_BASE + 0x18) // [sh(16) sw(16)]
#define SA16_REG_CONV_PAD       (SA16_REG_CONV_BASE + 0x1C) // [ph(16) pw(16)]
#define SA16_REG_CONV_CTRL      (SA16_REG_CONV_BASE + 0x20)
#define SA16_REG_CONV_STATUS    (SA16_REG_CONV_BASE + 0x24)
#define SA16_CONV_CTRL_START    (1u << 0)
#define SA16_CONV_STATUS_DONE   (1u << 0)

static void sa16_conv_issue(const uint8_t* input, const uint8_t* filter,
                            const uint8_t* bias, uint32_t N, uint32_t H,
                            uint32_t W, uint32_t C, uint32_t OC, uint32_t KH,
                            uint32_t KW, uint32_t stride_h, uint32_t stride_w,
                            uint32_t pad_h, uint32_t pad_w, bool apply_relu) {
    if (!axi_sa16) { DBG_E("SA16 not initialized!"); return; }

    // Assume f32 NHWC / OIHW layout for example (FIXME to RTL)
    size_t in_bytes   = (size_t)N*H*W*C*4;
    size_t fil_bytes  = (size_t)OC*KH*KW*C*4;
    size_t bias_bytes = (size_t)OC*4;

    DBG("CONV issue N=%u H=%u W=%u C=%u OC=%u KH=%u KW=%u sh=%u sw=%u ph=%u pw=%u relu=%d",
        N,H,W,C,OC,KH,KW,stride_h,stride_w,pad_h,pad_w,(int)apply_relu);

    write_mem_block(SA16_MEM_BASE + 0x40000, input,  (uint32_t)in_bytes);
    write_mem_block(SA16_MEM_BASE + 0x80000, filter, (uint32_t)fil_bytes);
    if (bias) write_mem_block(SA16_MEM_BASE + 0xC0000, bias, (uint32_t)bias_bytes);

    uint32_t nhwc = ((N & 0xFF) << 24) | ((H & 0xFF) << 16) | ((W & 0xFF) << 8) | (C & 0xFF);
    uint32_t ockwkh = ((OC & 0xFFF) << 20) | ((KW & 0x3FF) << 10) | (KH & 0x3FF);
    uint32_t stride = ((stride_h & 0xFFFF) << 16) | (stride_w & 0xFFFF);
    uint32_t pad    = ((pad_h & 0xFFFF) << 16) | (pad_w & 0xFFFF);

    reg_write(SA16_REG_CONV_IN_ADDR,   SA16_MEM_BASE + 0x40000);
    reg_write(SA16_REG_CONV_FIL_ADDR,  SA16_MEM_BASE + 0x80000);
    reg_write(SA16_REG_CONV_BIAS_ADDR, bias ? (SA16_MEM_BASE + 0xC0000) : 0);
    reg_write(SA16_REG_CONV_OUT_ADDR,  SA16_MEM_OUT);

    reg_write(SA16_REG_CONV_NHWC,      nhwc);
    reg_write(SA16_REG_CONV_OC_KW_KH,  ockwkh);
    reg_write(SA16_REG_CONV_STRIDE,    stride);
    reg_write(SA16_REG_CONV_PAD,       pad);

    reg_write(SA16_REG_CONV_CTRL, SA16_CONV_CTRL_START);
    tick_cycles(2);
    g_pending_conv_relu = apply_relu;

    DBG("CONV started");
}

extern "C" void sa16_conv_offload(
    const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t stride_h, uint32_t stride_w,
    uint32_t pad_h, uint32_t pad_w)
{
    sa16_conv_issue(input, filter, bias, N, H, W, C, OC, KH, KW,
                    stride_h, stride_w, pad_h, pad_w, /*apply_relu=*/false);
}

extern "C" void sa16_conv_relu_offload(
    const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t stride_h, uint32_t stride_w,
    uint32_t pad_h, uint32_t pad_w)
{
    sa16_conv_issue(input, filter, bias, N, H, W, C, OC, KH, KW,
                    stride_h, stride_w, pad_h, pad_w, /*apply_relu=*/true);
}

extern "C" void sa16_conv_wait_for_completion(
    uint8_t* output, uint32_t N, uint32_t OH, uint32_t OW, uint32_t OC) {

    if (!axi_sa16) { DBG_E("SA16 not initialized!"); return; }

    size_t out_bytes = (size_t)N * OH * OW * OC * 4;
    DBG("WAIT begin (CONV) expect_out=%zu bytes", out_bytes);

    while (true) {
        uint32_t s = reg_read(SA16_REG_CONV_STATUS);
        if (s & SA16_CONV_STATUS_DONE) break;
        tick_cycles(200);
    }
    read_mem_block(output, SA16_MEM_OUT, (uint32_t)out_bytes);
    if (g_pending_conv_relu && output) {
        float* out_f32 = (float*)output;
        uint32_t element_count = (uint32_t)(out_bytes / sizeof(float));
        for (uint32_t i = 0; i < element_count; ++i) {
            if (out_f32[i] < 0.0f) out_f32[i] = 0.0f;
        }
    }
    g_pending_conv_relu = false;

    DBG("WAIT done (CONV)");
}

//=========================
// DPI shim (IREE expects these names)
//=========================
extern "C" void iree_dpi_sa16_matmul_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N) {
    sa16_matmul_bias_add_offload(lhs, rhs, bias, M, K, N);
}

extern "C" void iree_dpi_sa16_matmul_bias_add_relu_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N) {
    sa16_matmul_bias_add_relu_offload(lhs, rhs, bias, M, K, N);
}

extern "C" void iree_dpi_sa16_conv_offload(
    const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t stride_h, uint32_t stride_w,
    uint32_t pad_h, uint32_t pad_w) {
    sa16_conv_offload(input, filter, bias, N, H, W, C, OC, KH, KW,
                      stride_h, stride_w, pad_h, pad_w);
}

extern "C" void iree_dpi_sa16_conv_relu_offload(
    const uint8_t* input, const uint8_t* filter, const uint8_t* bias,
    uint32_t N, uint32_t H, uint32_t W, uint32_t C, uint32_t OC,
    uint32_t KH, uint32_t KW, uint32_t stride_h, uint32_t stride_w,
    uint32_t pad_h, uint32_t pad_w) {
    sa16_conv_relu_offload(input, filter, bias, N, H, W, C, OC, KH, KW,
                           stride_h, stride_w, pad_h, pad_w);
}

extern "C" void iree_dpi_sa16_conv_wait_for_completion(
    uint8_t* output, uint32_t N, uint32_t OH, uint32_t OW, uint32_t OC) {
    sa16_conv_wait_for_completion(output, N, OH, OW, OC);
}

//=========================
// main
//=========================
int main(int argc, char **argv, char **env) {
    if(argc < 3) {
        printf("Usage: %s {receiverPort} {senderPort} [{address}]\n", argv[0]);
        exit(-1);
    }
    const char *address = argc < 4 ? "127.0.0.1" : argv[3];

    Verilated::commandArgs(argc, argv);
#if VM_TRACE
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("simx.vcd");
    DBG("VCD tracing enabled -> simx.vcd");
#endif
    DBG("MAIN: Init()");
    Init();

    // hard reset again (safety)
    top->rst = 0;
    for (int i = 0; i < 20; ++i) tick_cycles(1);
    top->rst = 1;
    for (int i = 0; i < 20; ++i) tick_cycles(1);
    DBG("MAIN: reset ok, entering simulate() on %s:%s/%s",
        address, argv[1], argv[2]);

    axi_sa16->simulate(atoi(argv[1]), atoi(argv[2]), address);
    DBG("simulate() returned, finalizing");

    top->final();
    DBG("exit(0)");
    exit(0);
}
