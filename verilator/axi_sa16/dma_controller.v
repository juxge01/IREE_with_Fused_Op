`timescale 1ns/1ps
`default_nettype none

// -----------------------------------------------------------------------------
// AXI4 READ -> Stream DMA Controller (AXI-Lite control)
// -----------------------------------------------------------------------------
module dma_controller #(
    parameter integer C_S_AXI_DATA_WIDTH = 32,
    parameter integer C_S_AXI_ADDR_WIDTH = 32,
    parameter integer C_M_AXI_DATA_WIDTH = 32,
    parameter integer C_M_AXI_ADDR_WIDTH = 32
)(
    // Global
    input  wire                              ACLK,
    input  wire                              ARESETn,

    // AXI4-Lite Slave (control)
    input  wire [C_S_AXI_ADDR_WIDTH-1:0]     S_AXI_AWADDR,
    input  wire                              S_AXI_AWVALID,
    output wire                              S_AXI_AWREADY,
    input  wire [C_S_AXI_DATA_WIDTH-1:0]     S_AXI_WDATA,
    input  wire [C_S_AXI_DATA_WIDTH/8-1:0]   S_AXI_WSTRB,
    input  wire                              S_AXI_WVALID,
    output wire                              S_AXI_WREADY,
    output wire [1:0]                        S_AXI_BRESP,
    output wire                              S_AXI_BVALID,
    input  wire                              S_AXI_BREADY,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0]     S_AXI_ARADDR,
    input  wire                              S_AXI_ARVALID,
    output wire                              S_AXI_ARREADY,
    output wire [C_S_AXI_DATA_WIDTH-1:0]     S_AXI_RDATA,
    output wire [1:0]                        S_AXI_RRESP,
    output wire                              S_AXI_RVALID,
    input  wire                              S_AXI_RREADY,

    // AXI4 Master Read
    output wire [C_M_AXI_ADDR_WIDTH-1:0]     M_AXI_ARADDR,
    output wire [7:0]                        M_AXI_ARLEN,
    output wire [2:0]                        M_AXI_ARSIZE,
    output wire [1:0]                        M_AXI_ARBURST,
    output wire                              M_AXI_ARVALID,
    input  wire                              M_AXI_ARREADY,
    input  wire [C_M_AXI_DATA_WIDTH-1:0]     M_AXI_RDATA,
    input  wire [1:0]                        M_AXI_RRESP,
    input  wire                              M_AXI_RLAST,
    input  wire                              M_AXI_RVALID,
    output wire                              M_AXI_RREADY,

    // Stream out
    output wire [C_M_AXI_DATA_WIDTH-1:0]     m_axis_data,
    output wire                              m_axis_valid,
    input  wire                              m_axis_ready,

    // Status
    output wire                              interrupt
);
    // ---------------- AXI-Lite regs ----------------
    localparam [C_S_AXI_ADDR_WIDTH-1:0] ADDR_AP_CTRL     = 'h00;
    localparam [C_S_AXI_ADDR_WIDTH-1:0] ADDR_SOURCE_ADDR = 'h10;
    localparam [C_S_AXI_ADDR_WIDTH-1:0] ADDR_LENGTH     = 'h18;
    localparam AP_START = 0, AP_DONE = 1, AP_IDLE = 2, AP_READY = 3;

    reg  axi_awready_reg, axi_wready_reg, axi_bvalid_reg;
    reg  [1:0] axi_bresp_reg;
    reg  axi_arready_reg, axi_rvalid_reg;
    reg  [1:0] axi_rresp_reg;
    reg  [C_S_AXI_DATA_WIDTH-1:0] axi_rdata_reg;
    reg  [C_S_AXI_ADDR_WIDTH-1:0] axi_awaddr, axi_araddr;

    assign S_AXI_AWREADY = axi_awready_reg;
    assign S_AXI_WREADY  = axi_wready_reg;
    assign S_AXI_BRESP   = axi_bresp_reg;
    assign S_AXI_BVALID  = axi_bvalid_reg;
    assign S_AXI_ARREADY = axi_arready_reg;
    assign S_AXI_RDATA   = axi_rdata_reg;
    assign S_AXI_RRESP   = axi_rresp_reg;
    assign S_AXI_RVALID  = axi_rvalid_reg;

    reg  ap_start_reg;
    wire ap_done, ap_idle, ap_ready;
    reg  [C_M_AXI_ADDR_WIDTH-1:0] source_addr_reg;
    reg  [31:0] length_reg;

    // AXI-lite write
    always @(posedge ACLK) begin
        if (!ARESETn) begin
            axi_awready_reg <= 1'b0; axi_wready_reg <= 1'b0;
            axi_bvalid_reg  <= 1'b0; axi_bresp_reg  <= 2'b0;
            axi_awaddr      <= '0;
        end else begin
            axi_awready_reg <= (~axi_awready_reg && S_AXI_AWVALID && S_AXI_WVALID);
            axi_wready_reg  <= (~axi_wready_reg  && S_AXI_AWVALID && S_AXI_WVALID);

            if (S_AXI_AWVALID && S_AXI_WVALID) axi_awaddr <= S_AXI_AWADDR;

            if (axi_bvalid_reg && S_AXI_BREADY) axi_bvalid_reg <= 1'b0;
            if (axi_awready_reg && S_AXI_AWVALID && axi_wready_reg && S_AXI_WVALID) begin
                axi_bvalid_reg <= 1'b1;
                axi_bresp_reg  <= 2'b00;
            end
        end
    end

    // AXI-lite reg write
    always @(posedge ACLK) begin
        if (!ARESETn) begin
            ap_start_reg    <= 1'b0;
            source_addr_reg <= '0;
            length_reg      <= '0;
        end else if (axi_awready_reg && S_AXI_AWVALID && axi_wready_reg && S_AXI_WVALID) begin
            case (axi_awaddr)
                ADDR_AP_CTRL:     ap_start_reg    <= S_AXI_WDATA[AP_START];
                ADDR_SOURCE_ADDR: source_addr_reg <= S_AXI_WDATA[C_M_AXI_ADDR_WIDTH-1:0];
                ADDR_LENGTH:      length_reg      <= S_AXI_WDATA;
            endcase
        end
    end

    // AXI-lite read
    always @(posedge ACLK) begin
        if (!ARESETn) begin
            axi_arready_reg <= 1'b0; axi_rvalid_reg <= 1'b0;
            axi_rresp_reg   <= 2'b0; axi_araddr     <= '0; axi_rdata_reg <= '0;
        end else begin
            axi_arready_reg <= (~axi_arready_reg && S_AXI_ARVALID);
            if (axi_arready_reg && S_AXI_ARVALID) axi_araddr <= S_AXI_ARADDR;

            if (axi_arready_reg && S_AXI_ARVALID && ~axi_rvalid_reg) begin
                axi_rvalid_reg <= 1'b1; axi_rresp_reg <= 2'b00;
                case (S_AXI_ARADDR)
                    ADDR_AP_CTRL:     axi_rdata_reg <= {28'b0, ap_ready, ap_idle, ap_done, ap_start_reg};
                    ADDR_SOURCE_ADDR: axi_rdata_reg <= source_addr_reg;
                    ADDR_LENGTH:      axi_rdata_reg <= length_reg;
                    default:          axi_rdata_reg <= '0;
                endcase
            end else if (axi_rvalid_reg && S_AXI_RREADY) begin
                axi_rvalid_reg <= 1'b0;
            end
        end
    end

    // ---------------- DMA core (AXI4 Read -> stream) ----------------
    localparam [1:0] S_IDLE=2'b00, S_SEND_ADDR=2'b01, S_READ=2'b10, S_DONE=2'b11;
    reg [1:0] state;

    reg [C_M_AXI_ADDR_WIDTH-1:0] current_addr;
    reg [31:0] remaining_bytes;

    reg arvalid_reg;
    reg [7:0] arlen_reg;                 // beats-1
    reg [7:0] beats_left_in_burst;       // runtime counter

    localparam integer BYTES_PER_BEAT = (C_M_AXI_DATA_WIDTH/8);
    localparam [2:0]  AXI_SIZE  = $clog2(BYTES_PER_BEAT);

    assign M_AXI_ARADDR  = current_addr;
    assign M_AXI_ARSIZE  = AXI_SIZE;
    assign M_AXI_ARBURST = 2'b01;        // INCR
    assign M_AXI_ARVALID = arvalid_reg;
    assign M_AXI_ARLEN   = arlen_reg;

    // stream out directly from R
    assign m_axis_data   = M_AXI_RDATA;
    assign m_axis_valid  = M_AXI_RVALID;
    assign M_AXI_RREADY  = m_axis_ready;

    reg interrupt_reg;
    assign interrupt = interrupt_reg;

    wire ap_idle_w = (state==S_IDLE);
    wire ap_done_w = (state==S_DONE);
    assign ap_idle  = ap_idle_w;
    assign ap_done  = ap_done_w;
    assign ap_ready = ap_idle_w | ap_done_w;

    always @(posedge ACLK) begin
        if (!ARESETn) begin
            state <= S_IDLE;
            arvalid_reg <= 1'b0;
            arlen_reg   <= 8'd0;
            beats_left_in_burst <= 8'd0;
            current_addr <= '0;
            remaining_bytes <= '0;
            interrupt_reg <= 1'b0;
            // ap_start stays low at reset
        end else begin
            case (state)
            S_IDLE: begin
                interrupt_reg <= 1'b0;
                if (ap_start_reg) begin
                    current_addr    <= source_addr_reg;
                    remaining_bytes <= length_reg;
                    state           <= S_SEND_ADDR;
                end
            end

            S_SEND_ADDR: begin
                if (remaining_bytes == 0) begin
                    state <= S_DONE;
                end else if (!arvalid_reg) begin
                    // compute beats for this burst: min(remaining/bytes_per_beat, 256)
                    // beats must be at least 1
                    integer beats;
                    beats = remaining_bytes / BYTES_PER_BEAT;
                    if (beats == 0) beats = 1;
                    if (beats > 256) beats = 256;
                    arlen_reg           <= beats[7:0]-1;
                    beats_left_in_burst <= beats[7:0];
                    arvalid_reg         <= 1'b1;
                end else if (arvalid_reg && M_AXI_ARREADY) begin
                    arvalid_reg   <= 1'b0;
                    state         <= S_READ;
                end
            end

            S_READ: begin
                if (M_AXI_RVALID && m_axis_ready) begin
                    // one beat consumed
                    current_addr     <= current_addr + BYTES_PER_BEAT;
                    remaining_bytes  <= (remaining_bytes > BYTES_PER_BEAT) ? (remaining_bytes - BYTES_PER_BEAT) : 0;
                    if (beats_left_in_burst != 0) beats_left_in_burst <= beats_left_in_burst - 1'b1;

                    if (M_AXI_RLAST) begin
                        // burst done
                        if (remaining_bytes <= BYTES_PER_BEAT) begin
                            // possibly zero already on next cycle; send DONE after last beat
                            if (remaining_bytes - BYTES_PER_BEAT == 0)
                                state <= S_DONE;
                            else
                                state <= S_SEND_ADDR;
                        end else begin
                            state <= S_SEND_ADDR;
                        end
                    end
                end
            end

            S_DONE: begin
                interrupt_reg <= 1'b1;
                // auto clear start to avoid immediate retrigger
                if (ap_start_reg) ap_start_reg <= 1'b0;
                state <= S_IDLE;
            end
            endcase
        end
    end
endmodule

`default_nettype wire
