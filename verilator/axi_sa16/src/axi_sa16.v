module axi_sa16 #
(
    parameter DATA_WIDTH = 32,
    parameter ADDR_WIDTH = 20,
    parameter STRB_WIDTH = (DATA_WIDTH/8),
    parameter ID_WIDTH   = 8,
    parameter PIPELINE_OUTPUT = 0
)
(
    input  wire                   clk,
    input  wire                   rst,

    // AXI4 write address channel
    input  wire [ID_WIDTH-1:0]    s_axi_awid,
    input  wire [ADDR_WIDTH-1:0]  s_axi_awaddr,
    input  wire [7:0]             s_axi_awlen,
    input  wire [2:0]             s_axi_awsize,
    input  wire [1:0]             s_axi_awburst,
    input  wire                   s_axi_awlock,
    input  wire [3:0]             s_axi_awcache,
    input  wire [2:0]             s_axi_awprot,
    input  wire                   s_axi_awvalid,
    output wire                   s_axi_awready,

    // AXI4 write data channel
    input  wire [DATA_WIDTH-1:0]  s_axi_wdata,
    input  wire [STRB_WIDTH-1:0]  s_axi_wstrb,
    input  wire                   s_axi_wlast,
    input  wire                   s_axi_wvalid,
    output wire                   s_axi_wready,

    // AXI4 write response channel
    output wire [ID_WIDTH-1:0]    s_axi_bid,
    output wire [1:0]             s_axi_bresp,
    output wire                   s_axi_bvalid,
    input  wire                   s_axi_bready,

    // AXI4 read address channel
    input  wire [ID_WIDTH-1:0]    s_axi_arid,
    input  wire [ADDR_WIDTH-1:0]  s_axi_araddr,
    input  wire [7:0]             s_axi_arlen,
    input  wire [2:0]             s_axi_arsize,
    input  wire [1:0]             s_axi_arburst,
    input  wire                   s_axi_arlock,
    input  wire [3:0]             s_axi_arcache,
    input  wire [2:0]             s_axi_arprot,
    input  wire                   s_axi_arvalid,
    output wire                   s_axi_arready,

    // AXI4 read data channel
    output wire [ID_WIDTH-1:0]    s_axi_rid,
    output wire [DATA_WIDTH-1:0]  s_axi_rdata,
    output wire [1:0]             s_axi_rresp,
    output wire                   s_axi_rlast,
    output wire                   s_axi_rvalid,
    input  wire                   s_axi_rready
);

    // ---------------- AXI RAM core (원형 최소 수정) ----------------
    localparam VALID_ADDR_WIDTH = ADDR_WIDTH - $clog2(STRB_WIDTH);
    localparam WORD_WIDTH = STRB_WIDTH;
    localparam WORD_SIZE  = DATA_WIDTH/WORD_WIDTH;

    // 상단 파라미터 근처에 추가
    localparam [ADDR_WIDTH-1:0] OUT_WIN_BASE = 20'h01000;  // 결과 읽는 창 (0x...1000)
    localparam [ADDR_WIDTH-1:0] FEED_BASE    = 20'h00000;  // 스트리밍 피드(기존처럼 아무 주소든 가능)

    localparam integer          BEAT_LSB     = $clog2(STRB_WIDTH); // word index LSB
    wire [VALID_ADDR_WIDTH-1:0] idx_r        = read_addr_reg[ADDR_WIDTH-1:BEAT_LSB];
    wire [VALID_ADDR_WIDTH-1:0] idx_w        = write_addr_reg[ADDR_WIDTH-1:BEAT_LSB];
    wire [VALID_ADDR_WIDTH-1:0] out_base_idx = OUT_WIN_BASE[ADDR_WIDTH-1:BEAT_LSB];


    initial begin
        if (WORD_SIZE * STRB_WIDTH != DATA_WIDTH) begin
            $error("Error: AXI data width not evenly divisble (instance %m)");
            $finish;
        end
        if (2**$clog2(WORD_WIDTH) != WORD_WIDTH) begin
            $error("Error: AXI word width must be even power of two (instance %m)");
            $finish;
        end
    end

    localparam [0:0]
        READ_STATE_IDLE  = 1'd0,
        READ_STATE_BURST = 1'd1;

    localparam [1:0]
        WRITE_STATE_IDLE  = 2'd0,
        WRITE_STATE_BURST = 2'd1,
        WRITE_STATE_RESP  = 2'd2;

    reg [0:0] read_state  = READ_STATE_IDLE,  read_state_next;
    reg [1:0] write_state = WRITE_STATE_IDLE, write_state_next;

    reg mem_wr_en, mem_rd_en;

    reg [ID_WIDTH-1:0]   read_id_reg = {ID_WIDTH{1'b0}},  read_id_next;
    reg [ADDR_WIDTH-1:0] read_addr_reg = {ADDR_WIDTH{1'b0}}, read_addr_next;
    reg [7:0]            read_count_reg = 8'd0, read_count_next;
    reg [2:0]            read_size_reg = 3'd0,  read_size_next;
    reg [1:0]            read_burst_reg = 2'd0, read_burst_next;

    reg [ID_WIDTH-1:0]   write_id_reg = {ID_WIDTH{1'b0}}, write_id_next;
    reg [ADDR_WIDTH-1:0] write_addr_reg = {ADDR_WIDTH{1'b0}}, write_addr_next;
    reg [7:0]            write_count_reg = 8'd0, write_count_next;
    reg [2:0]            write_size_reg = 3'd0,  write_size_next;
    reg [1:0]            write_burst_reg = 2'd0, write_burst_next;

    reg                  s_axi_awready_reg = 1'b0, s_axi_awready_next;
    reg                  s_axi_wready_reg  = 1'b0, s_axi_wready_next;
    reg [ID_WIDTH-1:0]   s_axi_bid_reg = {ID_WIDTH{1'b0}}, s_axi_bid_next;
    reg                  s_axi_bvalid_reg = 1'b0, s_axi_bvalid_next;

    reg                  s_axi_arready_reg = 1'b0, s_axi_arready_next;
    reg [ID_WIDTH-1:0]   s_axi_rid_reg = {ID_WIDTH{1'b0}}, s_axi_rid_next;
    reg [DATA_WIDTH-1:0] s_axi_rdata_reg = {DATA_WIDTH{1'b0}}, s_axi_rdata_next;
    reg                  s_axi_rlast_reg = 1'b0, s_axi_rlast_next;
    reg                  s_axi_rvalid_reg = 1'b0, s_axi_rvalid_next;
    reg [ID_WIDTH-1:0]   s_axi_rid_pipe_reg = {ID_WIDTH{1'b0}};
    reg [DATA_WIDTH-1:0] s_axi_rdata_pipe_reg = {DATA_WIDTH{1'b0}};
    reg                  s_axi_rlast_pipe_reg = 1'b0;
    reg                  s_axi_rvalid_pipe_reg = 1'b0;

    reg [DATA_WIDTH-1:0] data_mem[(2**VALID_ADDR_WIDTH)-1:0];
    reg [DATA_WIDTH-1:0] output_mem[(2**VALID_ADDR_WIDTH)-1:0];

    wire [VALID_ADDR_WIDTH-1:0] s_axi_awaddr_valid = s_axi_awaddr >> (ADDR_WIDTH - VALID_ADDR_WIDTH);
    wire [VALID_ADDR_WIDTH-1:0] s_axi_araddr_valid = s_axi_araddr >> (ADDR_WIDTH - VALID_ADDR_WIDTH);
    wire [VALID_ADDR_WIDTH-1:0] read_addr_valid  = read_addr_reg  >> (ADDR_WIDTH - VALID_ADDR_WIDTH);
    wire [VALID_ADDR_WIDTH-1:0] write_addr_valid = write_addr_reg >> (ADDR_WIDTH - VALID_ADDR_WIDTH);

    assign s_axi_awready = s_axi_awready_reg;
    assign s_axi_wready  = s_axi_wready_reg;
    assign s_axi_bid     = s_axi_bid_reg;
    assign s_axi_bresp   = 2'b00;
    assign s_axi_bvalid  = s_axi_bvalid_reg;

    assign s_axi_arready = s_axi_arready_reg;
    assign s_axi_rid     = PIPELINE_OUTPUT ? s_axi_rid_pipe_reg   : s_axi_rid_reg;
    assign s_axi_rdata   = PIPELINE_OUTPUT ? s_axi_rdata_pipe_reg : s_axi_rdata_reg;
    assign s_axi_rresp   = 2'b00;
    assign s_axi_rlast   = PIPELINE_OUTPUT ? s_axi_rlast_pipe_reg : s_axi_rlast_reg;
    assign s_axi_rvalid  = PIPELINE_OUTPUT ? s_axi_rvalid_pipe_reg: s_axi_rvalid_reg;

    integer i, j, k;

    initial begin
        for (i = 0; i < 2**VALID_ADDR_WIDTH; i = i + 2**(VALID_ADDR_WIDTH/2)) begin
            for (j = i; j < i + 2**(VALID_ADDR_WIDTH/2); j = j + 1) begin
                data_mem[j]   = '0;
                output_mem[j] = '0;
            end
        end
    end

    // WRITE FSM
    always @* begin
        // 기본값은 '유지'로!
        write_state_next   = write_state;
        mem_wr_en          = 1'b0;

        write_id_next      = write_id_reg;
        write_addr_next    = write_addr_reg;
        write_count_next   = write_count_reg;
        write_size_next    = write_size_reg;
        write_burst_next   = write_burst_reg;

        s_axi_awready_next = s_axi_awready_reg;
        s_axi_wready_next  = s_axi_wready_reg;
        s_axi_bid_next     = s_axi_bid_reg;
        s_axi_bvalid_next  = s_axi_bvalid_reg;

        case (write_state)
        WRITE_STATE_IDLE: begin
            s_axi_awready_next = 1'b1;
            // s_axi_wready_next  = 1'b0;
            s_axi_wready_next  = 1'b1;
            if (s_axi_awvalid && s_axi_awready) begin
            write_id_next    = s_axi_awid;
            write_addr_next  = s_axi_awaddr;
            write_count_next = s_axi_awlen;  // beats-1
            write_size_next  = (s_axi_awsize < $clog2(STRB_WIDTH)) ? s_axi_awsize : $clog2(STRB_WIDTH);
            write_burst_next = s_axi_awburst;
            s_axi_awready_next = 1'b0;
            s_axi_wready_next  = 1'b1;
            write_state_next   = WRITE_STATE_BURST;
            end
        end

        // WRITE_STATE_BURST: begin
        //     s_axi_wready_next = 1'b1;
        //     if (s_axi_wvalid && s_axi_wready) begin
        //     mem_wr_en = 1'b1;

        //     // 주소/카운터 진행
        //     if (write_burst_reg != 2'b00)
        //         write_addr_next = write_addr_reg + (1 << write_size_reg);

        //     // wlast 기준으로 종료 판정(더 안전)
        //     if (s_axi_wlast || (write_count_reg == 0)) begin
        //         s_axi_wready_next = 1'b0;
        //         if (s_axi_bready || !s_axi_bvalid) begin
        //         s_axi_bid_next    = write_id_reg;
        //         s_axi_bvalid_next = 1'b1;
        //         s_axi_awready_next= 1'b1;
        //         write_state_next  = WRITE_STATE_IDLE;
        //         end else begin
        //         write_state_next  = WRITE_STATE_RESP;
        //         end
        //     end else begin
        //         write_count_next = write_count_reg - 1;
        //     end
        //     end
        // end
        WRITE_STATE_BURST: begin
            s_axi_wready_next = 1'b1;

            if (s_axi_wvalid && s_axi_wready) begin
                mem_wr_en = 1'b1;

                // 주소 진행
                if (write_burst_reg != 2'b00)
                    write_addr_next = write_addr_reg + (1 << write_size_reg);

                // 마지막 beat (wlast 기준)
                if (s_axi_wlast) begin
                    s_axi_wready_next = 1'b1;
                    // BRESP 바로 발행
                    s_axi_bid_next    = write_id_reg;
                    s_axi_bvalid_next = 1'b1;
                    write_state_next  = WRITE_STATE_RESP;
                end else begin
                    write_count_next = write_count_reg - 1;
                end
            end
        end

        // WRITE_STATE_RESP: begin
        //     if (s_axi_bready || !s_axi_bvalid) begin
        //     s_axi_bid_next    = write_id_reg;
        //     s_axi_bvalid_next = 1'b1;
        //     s_axi_awready_next= 1'b1;
        //     write_state_next  = WRITE_STATE_IDLE;
        //     end
        // end
        WRITE_STATE_RESP: begin
            // master가 bready=1 해주면 handshake 완료
            if (s_axi_bvalid && s_axi_bready) begin
                s_axi_bvalid_next  = 1'b0;
                s_axi_awready_next = 1'b1;  // 다음 트랜잭션 받을 준비
                write_state_next   = WRITE_STATE_IDLE;
            end
        end
        endcase
    end

    always @(posedge clk) begin
        if (rst) begin
            write_state       <= WRITE_STATE_IDLE;
            s_axi_awready_reg <= 1'b0;
            s_axi_wready_reg  <= 1'b0;
            s_axi_bvalid_reg  <= 1'b0;
        end else begin
            write_state       <= write_state_next;
            s_axi_awready_reg <= s_axi_awready_next;
            s_axi_wready_reg  <= s_axi_wready_next;
            s_axi_bvalid_reg  <= s_axi_bvalid_next;
        end

        write_id_reg    <= write_id_next;
        write_addr_reg  <= write_addr_next;
        write_count_reg <= write_count_next;
        write_size_reg  <= write_size_next;
        write_burst_reg <= write_burst_next;
        s_axi_bid_reg   <= s_axi_bid_next;

        // for (i = 0; i < WORD_WIDTH; i = i + 1) begin
        //     if (mem_wr_en & s_axi_wstrb[i]) begin
        //         data_mem[write_addr_valid][WORD_SIZE*i +: WORD_SIZE] <= s_axi_wdata[WORD_SIZE*i +: WORD_SIZE];
        //     end
        // end
        if (mem_wr_en) begin
            if (write_addr_reg < OUT_WIN_BASE) begin
                integer i;
                for (i = 0; i < WORD_WIDTH; i = i + 1) begin
                    if (s_axi_wstrb[i]) begin
                        data_mem[idx_w][WORD_SIZE*i +: WORD_SIZE] <= s_axi_wdata[WORD_SIZE*i +: WORD_SIZE];

                        // data_mem[ write_addr_reg[ADDR_WIDTH-1:BEAT_LSB] ][WORD_SIZE*i +: WORD_SIZE]
                        //     <= s_axi_wdata[WORD_SIZE*i +: WORD_SIZE];
                    end
                end
            end
            // else: OUT_WIN_BASE 이상은 무시 (필요하면 SLVERR 처리 가능)
        end
    end

    // READ FSM
    always @* begin
        read_state_next    = read_state;                 // hold
        mem_rd_en          = 1'b0;

        s_axi_rid_next     = s_axi_rid_reg;
        s_axi_rlast_next   = s_axi_rlast_reg;
        s_axi_rvalid_next  = s_axi_rvalid_reg && !(s_axi_rready || (PIPELINE_OUTPUT && !s_axi_rvalid_pipe_reg));

        read_id_next       = read_id_reg;
        read_addr_next     = read_addr_reg;
        read_count_next    = read_count_reg;
        read_size_next     = read_size_reg;
        read_burst_next    = read_burst_reg;

        s_axi_arready_next = s_axi_arready_reg;

        case (read_state)
        READ_STATE_IDLE: begin
            s_axi_arready_next = 1'b1;
            if (s_axi_arvalid && s_axi_arready) begin
            read_id_next    = s_axi_arid;
            read_addr_next  = s_axi_araddr;
            read_count_next = s_axi_arlen;
            read_size_next  = (s_axi_arsize < $clog2(STRB_WIDTH)) ? s_axi_arsize : $clog2(STRB_WIDTH);
            read_burst_next = s_axi_arburst;
            s_axi_arready_next = 1'b0;
            read_state_next    = READ_STATE_BURST;
            end
        end

        READ_STATE_BURST: begin
            if (s_axi_rready || (PIPELINE_OUTPUT && !s_axi_rvalid_pipe_reg) || !s_axi_rvalid_reg) begin
            mem_rd_en          = 1'b1;
            s_axi_rvalid_next  = 1'b1;
            s_axi_rid_next     = read_id_reg;
            s_axi_rlast_next   = (read_count_reg == 0);

            if (read_burst_reg != 2'b00)
                read_addr_next = read_addr_reg + (1 << read_size_reg);

            if (read_count_reg == 0) begin
                s_axi_arready_next = 1'b1;
                read_state_next    = READ_STATE_IDLE;
            end else begin
                read_count_next = read_count_reg - 1;
            end
            end
        end
        endcase
    end

    always @(posedge clk) begin
        if (rst) begin
            read_state           <= READ_STATE_IDLE;
            s_axi_arready_reg    <= 1'b0;
            s_axi_rvalid_reg     <= 1'b0;
            s_axi_rvalid_pipe_reg<= 1'b0;
        end else begin
            read_state           <= read_state_next;
            s_axi_arready_reg    <= s_axi_arready_next;
            s_axi_rvalid_reg     <= s_axi_rvalid_next;

            if (!s_axi_rvalid_pipe_reg || s_axi_rready) begin
                s_axi_rvalid_pipe_reg <= s_axi_rvalid_reg;
            end
        end

        read_id_reg    <= read_id_next;
        read_addr_reg  <= read_addr_next;
        read_count_reg <= read_count_next;
        read_size_reg  <= read_size_next;
        read_burst_reg <= read_burst_next;

        s_axi_rid_reg  <= s_axi_rid_next;
        s_axi_rlast_reg<= s_axi_rlast_next;

        // if (mem_rd_en) begin
        //     s_axi_rdata_reg <= data_mem[read_addr_valid];
        // end
        // if (mem_rd_en) begin
        //     if (read_addr_reg >= OUT_WIN_BASE)
        //         s_axi_rdata_reg <= output_mem[ read_addr_reg[ADDR_WIDTH-1:BEAT_LSB] ];
        //     else
        //         s_axi_rdata_reg <= data_mem  [ read_addr_reg[ADDR_WIDTH-1:BEAT_LSB] ];
        // end
        if (mem_rd_en) begin
            if (read_addr_reg >= OUT_WIN_BASE)
                s_axi_rdata_reg <= output_mem[idx_r - out_base_idx];  // ★ 베이스 빼기
            else
                s_axi_rdata_reg <= data_mem[idx_r];
        end
            

        if (!s_axi_rvalid_pipe_reg || s_axi_rready) begin
            s_axi_rid_pipe_reg   <= s_axi_rid_reg;
            s_axi_rdata_pipe_reg <= s_axi_rdata_reg;
            s_axi_rlast_pipe_reg <= s_axi_rlast_reg;
        end
    end

    // ---------------- Systolic array streaming glue ----------------
    localparam NUM_INPUTA  = 16;
    localparam NUM_INPUTB  = 64;
    localparam NUM_OUTPUTC = 64;

    // BYPASS 제어 및 누산 버퍼 (NUM_* 선언 뒤에 위치)
    localparam integer BYPASS_SA = 1; // 1이면 SA 대신 bypassC 저장(연결 검증용)
    // localparam integer BYPASS_SA = 0; // 1이면 SA 대신 bypassC 저장(연결 검증용)
    reg [19:0] bypassC [0:NUM_OUTPUTC-1];

    // Ping-Pong Banks
    reg [7:0] inputA_bank0 [0:NUM_INPUTA-1];
    reg [7:0] inputA_bank1 [0:NUM_INPUTA-1];
    reg [7:0] inputB_bank0 [0:NUM_INPUTB-1];
    reg [7:0] inputB_bank1 [0:NUM_INPUTB-1];

    // SA I/O
    wire  [7:0]  inputA [0:NUM_INPUTA-1];
    wire  [7:0]  inputB [0:NUM_INPUTB-1];
    wire         propagateB [0:15];
    wire [19:0] outputC[0:NUM_OUTPUTC-1];

    genvar gi;
    generate
        for (gi=0; gi<16; gi=gi+1)  assign inputA[gi]     = A_buf[gi];
        for (gi=0; gi<64; gi=gi+1)  assign inputB[gi]     = B_buf[gi];
        for (gi=0; gi<16; gi=gi+1)  assign propagateB[gi] = 1'b1;
    endgenerate

    // integer k;
    // initial begin
    //     for (k = 0; k < 16; k = k + 1) propagateB[k] = 1'b1;
    // end

    // 디버그 카운터
    reg [31:0] counter;
    always @(posedge clk or posedge rst) begin
        if (rst) counter <= 0; else counter <= counter + 1;
    end

    // 수집 제어
    reg       bank_load;      // 0 -> bank0, 1 -> bank1 로드 중
    reg [6:0] load_count;     // 0..79 (A16+B64)
    reg       tile_ready0, tile_ready1;

    wire w_hs = s_axi_wvalid && s_axi_wready;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            bank_load   <= 1'b0;
            load_count  <= 7'd0;
            tile_ready0 <= 1'b0;
            tile_ready1 <= 1'b0;
        end else if (w_hs) begin
            if (load_count < NUM_INPUTA) begin
                if (!bank_load) inputA_bank0[load_count] <= s_axi_wdata[7:0];
                else            inputA_bank1[load_count] <= s_axi_wdata[7:0];
                load_count <= load_count + 1'b1;
            end else if (load_count < (NUM_INPUTA + NUM_INPUTB)) begin
                if (!bank_load) inputB_bank0[load_count - NUM_INPUTA] <= s_axi_wdata[7:0];
                else            inputB_bank1[load_count - NUM_INPUTA] <= s_axi_wdata[7:0];
                load_count <= load_count + 1'b1;
            end

            if (load_count == (NUM_INPUTA + NUM_INPUTB - 1)) begin
                if (!bank_load) tile_ready0 <= 1'b1; else tile_ready1 <= 1'b1;
                load_count <= 7'd0;
                bank_load  <= ~bank_load;  // 다음 타일은 반대 뱅크에 적재
            end
        end
    end

    // 컴퓨트 상태기계 (ARMED→RUN 지연)
    localparam S_IDLE=2'd0, S_ARMED=2'd1, S_RUN=2'd2;
    reg [1:0] sa_state;
    reg bank_comp;           // 현재 컴퓨트 중인 뱅크
    reg sa_busy, sa_start_pulse;

    // 버퍼: SA에 실제 물리는 A/B
    reg [7:0]  A_buf [0:NUM_INPUTA-1];
    reg [7:0]  B_buf [0:NUM_INPUTB-1];

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            sa_state       <= S_IDLE;
            bank_comp      <= 1'b1;
            sa_busy        <= 1'b0;
            sa_start_pulse <= 1'b0;
        end else begin
            sa_start_pulse <= 1'b0;
            case (sa_state)
                S_IDLE: begin
                    sa_busy <= 1'b0;

                    if ((!bank_comp && tile_ready0) || (bank_comp && tile_ready1)) begin
                        integer t;
                        // A 핑퐁 복사
                        if (!bank_comp) begin
                            for (t=0; t<NUM_INPUTA; t=t+1) A_buf[t] <= inputA_bank0[t];
                            tile_ready0 <= 1'b0;
                        end else begin
                            for (t=0; t<NUM_INPUTA; t=t+1) A_buf[t] <= inputA_bank1[t];
                            tile_ready1 <= 1'b0;
                        end
                        // *** 매 타일마다 B 복원(중요) ***
                        for (t=0; t<NUM_INPUTB; t=t+1) B_buf[t] <= inputB_bank0[t];

                        sa_state <= S_ARMED;

                    end else if ((bank_comp && tile_ready0) || (!bank_comp && tile_ready1)) begin
                        bank_comp <= ~bank_comp;   // 반대 뱅크 우선권
                    end
                end

                S_ARMED: begin
                    sa_start_pulse <= 1'b1;
                    sa_busy        <= 1'b1;
                    sa_state       <= S_RUN;
                end

                S_RUN: begin
                    if (final_output_detected) sa_state <= S_IDLE;
                end
            endcase
        end
    end

    // SA 포트 드라이브는 버퍼에서
    // always @* begin
    //     integer t2;
    //     for (t2=0; t2<NUM_INPUTA; t2=t2+1) inputA[t2] = A_buf[t2];
    //     for (t2=0; t2<NUM_INPUTB; t2=t2+1) inputB[t2] = B_buf[t2];
    // end

    // 안정화 검출 (워밍업 + 최소 연산 싸이클 보장)
    reg [19:0] prev_outputC [0:NUM_OUTPUTC-1];
    reg [5:0]  stable_cycles;
    reg        final_output_detected;
    reg        stable_outputC_result;

    reg [6:0]  warmup_cycles;
    reg [9:0]  compute_cycles, compute_cycles_req;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            warmup_cycles      <= 0;
            compute_cycles     <= 0;
            compute_cycles_req <= 10'd128; // 필요시 256 등으로 조정
        end else begin
            if (sa_start_pulse) begin
                warmup_cycles  <= 7'd64;    // 파이프라인 워밍업
                compute_cycles <= 0;
            end else if (sa_busy && warmup_cycles!=0) begin
                warmup_cycles  <= warmup_cycles - 1'b1;
            end
            if (sa_state==S_RUN && !final_output_detected) begin
                compute_cycles <= compute_cycles + 1'b1;
            end
        end
    end

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            stable_cycles         <= 0;
            final_output_detected <= 1'b0;
            stable_outputC_result <= 1'b0;
            for (k = 0; k < NUM_OUTPUTC; k = k + 1) prev_outputC[k] <= '0;
        end else begin
            if (sa_start_pulse) begin
                stable_cycles         <= 0;
                final_output_detected <= 1'b0;
                stable_outputC_result <= 1'b0;
                for (k = 0; k < NUM_OUTPUTC; k = k + 1) prev_outputC[k] <= '0;
                $display("[%t] Tile compute start (bank %0d) clk=%0d", $time, bank_comp, counter);
                $display("A_bank[%0d]= %0d %0d %0d %0d %0d %0d %0d %0d",
                    bank_comp?1:0,
                    (!bank_comp?inputA_bank0[0]:inputA_bank1[0]),
                    (!bank_comp?inputA_bank0[1]:inputA_bank1[1]),
                    (!bank_comp?inputA_bank0[2]:inputA_bank1[2]),
                    (!bank_comp?inputA_bank0[3]:inputA_bank1[3]),
                    (!bank_comp?inputA_bank0[4]:inputA_bank1[4]),
                    (!bank_comp?inputA_bank0[5]:inputA_bank1[5]),
                    (!bank_comp?inputA_bank0[6]:inputA_bank1[6]),
                    (!bank_comp?inputA_bank0[7]:inputA_bank1[7]));
                $display("B_bank[%0d]= %0d %0d %0d %0d %0d %0d %0d %0d",
                    bank_comp?1:0,
                    (!bank_comp?inputB_bank0[0]:inputB_bank1[0]),
                    (!bank_comp?inputB_bank0[1]:inputB_bank1[1]),
                    (!bank_comp?inputB_bank0[2]:inputB_bank1[2]),
                    (!bank_comp?inputB_bank0[3]:inputB_bank1[3]),
                    (!bank_comp?inputB_bank0[4]:inputB_bank1[4]),
                    (!bank_comp?inputB_bank0[5]:inputB_bank1[5]),
                    (!bank_comp?inputB_bank0[6]:inputB_bank1[6]),
                    (!bank_comp?inputB_bank0[7]:inputB_bank1[7]));
            end else if (sa_busy && !final_output_detected && warmup_cycles==0 && compute_cycles>=compute_cycles_req) begin
                integer eq_cnt; eq_cnt = 0;
                for (k = 0; k < NUM_OUTPUTC; k = k + 1) begin
                    if (outputC[k] == prev_outputC[k]) eq_cnt = eq_cnt + 1;
                    prev_outputC[k] <= outputC[k];
                end
                if (eq_cnt == NUM_OUTPUTC) stable_cycles <= stable_cycles + 1'b1; else stable_cycles <= 0;
                if (stable_cycles >= 6'd5) begin
                    final_output_detected <= 1'b1;
                    stable_outputC_result <= 1'b1;
                    $display("[%t] Tile stabilized (bank %0d) clk=%0d", $time, bank_comp, counter);
                end
            end
        end
    end

    reg b_loaded;    

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            b_loaded   <= 1'b0;
            bank_load  <= 1'b0;      // A 핑퐁
            load_count <= 7'd0;
            tile_ready0 <= 1'b0;
            tile_ready1 <= 1'b0;
        end else if (w_hs) begin
            if (!b_loaded) begin
                inputB_bank0[load_count] <= s_axi_wdata[7:0];
                inputB_bank1[load_count] <= s_axi_wdata[7:0];  // 추가: bank1도 같이 채움
                load_count <= load_count + 1'b1;
                if (load_count == 7'd63) begin
                    b_loaded   <= 1'b1;
                    load_count <= 7'd0;
                end
            end else begin
                // ---- 이후는 타일마다 A 16개만 로드 (핑퐁) ----
                if (!bank_load) inputA_bank0[load_count] <= s_axi_wdata[7:0];
                else            inputA_bank1[load_count] <= s_axi_wdata[7:0];
                load_count <= load_count + 1'b1;

                if (load_count == 7'd15) begin
                    if (!bank_load) tile_ready0 <= 1'b1; else tile_ready1 <= 1'b1;
                    bank_load  <= ~bank_load;
                    load_count <= 7'd0;
                end
            end
        end
    end

    // 바이패스 누산 (워밍업 이후)
    integer c;
    integer t;

    // always @(posedge clk or posedge rst) begin
    //     if (rst) begin
    //         for (t=0; t<NUM_INPUTB; t=t+1) B_buf[t] <= 8'd0;
    //     end else if (sa_state==S_RUN && !final_output_detected && warmup_cycles==0) begin
    //         integer t;
    //         for (t=NUM_INPUTB-1; t>0; t=t-1) B_buf[t] <= B_buf[t-1];
    //         B_buf[0] <= 8'd0; // 필요시 순환 시프트로 교체
    //     end
    // end

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            for (c=0;c<NUM_OUTPUTC;c=c+1) bypassC[c] <= 20'd0;
        end else if (sa_start_pulse) begin
            for (c=0;c<NUM_OUTPUTC;c=c+1) bypassC[c] <= 20'd0;
        end else if (sa_state==S_RUN && !final_output_detected && warmup_cycles==0) begin
            for (c=0;c<NUM_OUTPUTC;c=c+1)
                bypassC[c] <= bypassC[c] + A_buf[c%NUM_INPUTA] * B_buf[c];
        end
    end

    // 디버그: 누산 확인
    // integer dbg;
    // always @(posedge clk) begin
    //     if (sa_state==S_RUN && warmup_cycles==0 && (compute_cycles[3:0]==4'd0)) begin
    //         $display("bypassC[0..7]= %0d %0d %0d %0d %0d %0d %0d %0d",
    //             bypassC[0],bypassC[1],bypassC[2],bypassC[3],bypassC[4],bypassC[5],bypassC[6],bypassC[7]);
    //     end
    // end

    // 최종 저장: final_output_detected 상승엣지에서 스냅샷 저장
    reg final_q; always @(posedge clk or posedge rst) if (rst) final_q <= 1'b0; else final_q <= final_output_detected;
    wire final_rise = final_output_detected & ~final_q;

    reg [19:0] bypassC_snap [0:NUM_OUTPUTC-1];
    integer si;
    always @(posedge clk) if (final_rise) begin
        for (si=0; si<NUM_OUTPUTC; si=si+1) bypassC_snap[si] <= bypassC[si];
    end

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            for (i = 0; i < NUM_OUTPUTC; i = i + 1) output_mem[i] <= '0;
        end else if (final_rise) begin
            for (i = 0; i < NUM_OUTPUTC; i = i + 1)
                // output_mem[i] <= {{(DATA_WIDTH-20){1'b0}}, (BYPASS_SA ? bypassC_snap[i] : outputC[i])};
                output_mem[i] <= {{(DATA_WIDTH-20){1'b0}}, (BYPASS_SA ? bypassC[i] : outputC[i])};
            $display("[%t] Tile written to output_mem[0..63]", $time);
        end
    end

    // SA 인스턴스 (외부 제공)
    ws_sta_16x16x1x4x1 uut (
        .clock(clk),
        .reset(rst),
        // A
        .io_inputA_0(inputA[0]),   .io_inputA_1(inputA[1]),   .io_inputA_2(inputA[2]),   .io_inputA_3(inputA[3]),
        .io_inputA_4(inputA[4]),   .io_inputA_5(inputA[5]),   .io_inputA_6(inputA[6]),   .io_inputA_7(inputA[7]),
        .io_inputA_8(inputA[8]),   .io_inputA_9(inputA[9]),   .io_inputA_10(inputA[10]), .io_inputA_11(inputA[11]),
        .io_inputA_12(inputA[12]), .io_inputA_13(inputA[13]), .io_inputA_14(inputA[14]), .io_inputA_15(inputA[15]),
        // B
        .io_inputB_0(inputB[0]),   .io_inputB_1(inputB[1]),   .io_inputB_2(inputB[2]),   .io_inputB_3(inputB[3]),
        .io_inputB_4(inputB[4]),   .io_inputB_5(inputB[5]),   .io_inputB_6(inputB[6]),   .io_inputB_7(inputB[7]),
        .io_inputB_8(inputB[8]),   .io_inputB_9(inputB[9]),   .io_inputB_10(inputB[10]), .io_inputB_11(inputB[11]),
        .io_inputB_12(inputB[12]), .io_inputB_13(inputB[13]), .io_inputB_14(inputB[14]), .io_inputB_15(inputB[15]),
        .io_inputB_16(inputB[16]), .io_inputB_17(inputB[17]), .io_inputB_18(inputB[18]), .io_inputB_19(inputB[19]),
        .io_inputB_20(inputB[20]), .io_inputB_21(inputB[21]), .io_inputB_22(inputB[22]), .io_inputB_23(inputB[23]),
        .io_inputB_24(inputB[24]), .io_inputB_25(inputB[25]), .io_inputB_26(inputB[26]), .io_inputB_27(inputB[27]),
        .io_inputB_28(inputB[28]), .io_inputB_29(inputB[29]), .io_inputB_30(inputB[30]), .io_inputB_31(inputB[31]),
        .io_inputB_32(inputB[32]), .io_inputB_33(inputB[33]), .io_inputB_34(inputB[34]), .io_inputB_35(inputB[35]),
        .io_inputB_36(inputB[36]), .io_inputB_37(inputB[37]), .io_inputB_38(inputB[38]), .io_inputB_39(inputB[39]),
        .io_inputB_40(inputB[40]), .io_inputB_41(inputB[41]), .io_inputB_42(inputB[42]), .io_inputB_43(inputB[43]),
        .io_inputB_44(inputB[44]), .io_inputB_45(inputB[45]), .io_inputB_46(inputB[46]), .io_inputB_47(inputB[47]),
        .io_inputB_48(inputB[48]), .io_inputB_49(inputB[49]), .io_inputB_50(inputB[50]), .io_inputB_51(inputB[51]),
        .io_inputB_52(inputB[52]), .io_inputB_53(inputB[53]), .io_inputB_54(inputB[54]), .io_inputB_55(inputB[55]),
        .io_inputB_56(inputB[56]), .io_inputB_57(inputB[57]), .io_inputB_58(inputB[58]), .io_inputB_59(inputB[59]),
        .io_inputB_60(inputB[60]), .io_inputB_61(inputB[61]), .io_inputB_62(inputB[62]), .io_inputB_63(inputB[63]),
        // propagateB
        .io_propagateB_0(propagateB[0]), .io_propagateB_1(propagateB[1]), .io_propagateB_2(propagateB[2]), .io_propagateB_3(propagateB[3]),
        .io_propagateB_4(propagateB[4]), .io_propagateB_5(propagateB[5]), .io_propagateB_6(propagateB[6]), .io_propagateB_7(propagateB[7]),
        .io_propagateB_8(propagateB[8]), .io_propagateB_9(propagateB[9]), .io_propagateB_10(propagateB[10]), .io_propagateB_11(propagateB[11]),
        .io_propagateB_12(propagateB[12]), .io_propagateB_13(propagateB[13]), .io_propagateB_14(propagateB[14]), .io_propagateB_15(propagateB[15]),
        // outputC
        .io_outputC_0(outputC[0]),   .io_outputC_1(outputC[1]),   .io_outputC_2(outputC[2]),   .io_outputC_3(outputC[3]),
        .io_outputC_4(outputC[4]),   .io_outputC_5(outputC[5]),   .io_outputC_6(outputC[6]),   .io_outputC_7(outputC[7]),
        .io_outputC_8(outputC[8]),   .io_outputC_9(outputC[9]),   .io_outputC_10(outputC[10]), .io_outputC_11(outputC[11]),
        .io_outputC_12(outputC[12]), .io_outputC_13(outputC[13]), .io_outputC_14(outputC[14]), .io_outputC_15(outputC[15]),
        .io_outputC_16(outputC[16]), .io_outputC_17(outputC[17]), .io_outputC_18(outputC[18]), .io_outputC_19(outputC[19]),
        .io_outputC_20(outputC[20]), .io_outputC_21(outputC[21]), .io_outputC_22(outputC[22]), .io_outputC_23(outputC[23]),
        .io_outputC_24(outputC[24]), .io_outputC_25(outputC[25]), .io_outputC_26(outputC[26]), .io_outputC_27(outputC[27]),
        .io_outputC_28(outputC[28]), .io_outputC_29(outputC[29]), .io_outputC_30(outputC[30]), .io_outputC_31(outputC[31]),
        .io_outputC_32(outputC[32]), .io_outputC_33(outputC[33]), .io_outputC_34(outputC[34]), .io_outputC_35(outputC[35]),
        .io_outputC_36(outputC[36]), .io_outputC_37(outputC[37]), .io_outputC_38(outputC[38]), .io_outputC_39(outputC[39]),
        .io_outputC_40(outputC[40]), .io_outputC_41(outputC[41]), .io_outputC_42(outputC[42]), .io_outputC_43(outputC[43]),
        .io_outputC_44(outputC[44]), .io_outputC_45(outputC[45]), .io_outputC_46(outputC[46]), .io_outputC_47(outputC[47]),
        .io_outputC_48(outputC[48]), .io_outputC_49(outputC[49]), .io_outputC_50(outputC[50]), .io_outputC_51(outputC[51]),
        .io_outputC_52(outputC[52]), .io_outputC_53(outputC[53]), .io_outputC_54(outputC[54]), .io_outputC_55(outputC[55]),
        .io_outputC_56(outputC[56]), .io_outputC_57(outputC[57]), .io_outputC_58(outputC[58]), .io_outputC_59(outputC[59]),
        .io_outputC_60(outputC[60]), .io_outputC_61(outputC[61]), .io_outputC_62(outputC[62]), .io_outputC_63(outputC[63])
    );

`ifndef SYNTHESIS
    reg printed;
    reg print_pending;  // NEW: write 이후 다음 싸이클에 출력

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            printed       <= 1'b0;
            print_pending <= 1'b0;
        end else begin
            // final_rise에서 저장하고, 출력은 한 싸이클 뒤에 하도록 arm
            if (final_rise) begin
                printed       <= 1'b0;
                print_pending <= 1'b1;
            end else if (print_pending) begin
                integer n, remain;
                $display("\nComputation Complete. OutputC Values:");
                for (n = 0; n < NUM_OUTPUTC; n = n + 8) begin
                    remain = NUM_OUTPUTC - n;
                    if (remain >= 8) begin
                        $display("%08X %08X %08X %08X %08X %08X %08X %08X",
                            output_mem[n],   output_mem[n+1], output_mem[n+2], output_mem[n+3],
                            output_mem[n+4], output_mem[n+5], output_mem[n+6], output_mem[n+7]);
                    end else begin
                        case (remain)
                            1: $display("%08X", output_mem[n]);
                            2: $display("%08X %08X", output_mem[n], output_mem[n+1]);
                            3: $display("%08X %08X %08X", output_mem[n], output_mem[n+1], output_mem[n+2]);
                            4: $display("%08X %08X %08X %08X", output_mem[n], output_mem[n+1], output_mem[n+2], output_mem[n+3]);
                            5: $display("%08X %08X %08X %08X %08X", output_mem[n], output_mem[n+1], output_mem[n+2], output_mem[n+3], output_mem[n+4]);
                            6: $display("%08X %08X %08X %08X %08X %08X", output_mem[n], output_mem[n+1], output_mem[n+2], output_mem[n+3], output_mem[n+4], output_mem[n+5]);
                            7: $display("%08X %08X %08X %08X %08X %08X %08X", output_mem[n], output_mem[n+1], output_mem[n+2], output_mem[n+3], output_mem[n+4], output_mem[n+5], output_mem[n+6]);
                        endcase
                    end
                end
                printed       <= 1'b1;
                print_pending <= 1'b0;  // one-shot
            end
        end
    end
// `endif

    // always @(posedge clk) begin
    //     if (sa_start_pulse) begin
    //         $display("A[0..7]= %0d %0d %0d %0d %0d %0d %0d %0d",
    //             inputA[0],inputA[1],inputA[2],inputA[3],inputA[4],inputA[5],inputA[6],inputA[7]);
    //         $display("B[0..7]= %0d %0d %0d %0d %0d %0d %0d %0d",
    //             inputB[0],inputB[1],inputB[2],inputB[3],inputB[4],inputB[5],inputB[6],inputB[7]);
    //     end
    // end

    // integer bb; 
    // always @(posedge clk) begin
    // if (sa_start_pulse) begin
    //     $display("A[0..15]= %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d %0d",
    //     A_buf[0],A_buf[1],A_buf[2],A_buf[3],A_buf[4],A_buf[5],A_buf[6],A_buf[7],
    //     A_buf[8],A_buf[9],A_buf[10],A_buf[11],A_buf[12],A_buf[13],A_buf[14],A_buf[15]);

    //     for (bb=0; bb<64; bb=bb+8) begin
    //         $display("B[%0d..%0d]= %0d %0d %0d %0d %0d %0d %0d %0d",
    //             bb, bb+7,
    //             B_buf[bb],B_buf[bb+1],B_buf[bb+2],B_buf[bb+3],
    //             B_buf[bb+4],B_buf[bb+5],B_buf[bb+6],B_buf[bb+7]);
    //         end
    //     end
    // end
`endif

endmodule
`default_nettype wire

