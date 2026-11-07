// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1ns/1ps
module fp_arraygen_loop_control_s_axi
#(parameter
    C_S_AXI_ADDR_WIDTH = 7,
    C_S_AXI_DATA_WIDTH = 32
)(
    input  wire                          ACLK,
    input  wire                          ARESET,
    input  wire                          ACLK_EN,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] AWADDR,
    input  wire                          AWVALID,
    output wire                          AWREADY,
    input  wire [C_S_AXI_DATA_WIDTH-1:0] WDATA,
    input  wire [C_S_AXI_DATA_WIDTH/8-1:0] WSTRB,
    input  wire                          WVALID,
    output wire                          WREADY,
    output wire [1:0]                    BRESP,
    output wire                          BVALID,
    input  wire                          BREADY,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] ARADDR,
    input  wire                          ARVALID,
    output wire                          ARREADY,
    output wire [C_S_AXI_DATA_WIDTH-1:0] RDATA,
    output wire [1:0]                    RRESP,
    output wire                          RVALID,
    input  wire                          RREADY,
    output wire [7:0]                    mode,
    output wire [31:0]                   output_scale,
    output wire [63:0]                   w1,
    output wire [63:0]                   w2,
    output wire [63:0]                   w3,
    output wire [63:0]                   w4
);
//------------------------Address Info-------------------
// 0x00 : reserved
// 0x04 : reserved
// 0x08 : reserved
// 0x0c : reserved
// 0x10 : Data signal of mode
//        bit 7~0 - mode[7:0] (Read/Write)
//        others  - reserved
// 0x14 : reserved
// 0x18 : Data signal of output_scale
//        bit 31~0 - output_scale[31:0] (Read/Write)
// 0x1c : reserved
// 0x20 : Data signal of w1
//        bit 31~0 - w1[31:0] (Read/Write)
// 0x24 : Data signal of w1
//        bit 31~0 - w1[63:32] (Read/Write)
// 0x28 : reserved
// 0x2c : Data signal of w2
//        bit 31~0 - w2[31:0] (Read/Write)
// 0x30 : Data signal of w2
//        bit 31~0 - w2[63:32] (Read/Write)
// 0x34 : reserved
// 0x38 : Data signal of w3
//        bit 31~0 - w3[31:0] (Read/Write)
// 0x3c : Data signal of w3
//        bit 31~0 - w3[63:32] (Read/Write)
// 0x40 : reserved
// 0x44 : Data signal of w4
//        bit 31~0 - w4[31:0] (Read/Write)
// 0x48 : Data signal of w4
//        bit 31~0 - w4[63:32] (Read/Write)
// 0x4c : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

//------------------------Parameter----------------------
localparam
    ADDR_MODE_DATA_0         = 7'h10,
    ADDR_MODE_CTRL           = 7'h14,
    ADDR_OUTPUT_SCALE_DATA_0 = 7'h18,
    ADDR_OUTPUT_SCALE_CTRL   = 7'h1c,
    ADDR_W1_DATA_0           = 7'h20,
    ADDR_W1_DATA_1           = 7'h24,
    ADDR_W1_CTRL             = 7'h28,
    ADDR_W2_DATA_0           = 7'h2c,
    ADDR_W2_DATA_1           = 7'h30,
    ADDR_W2_CTRL             = 7'h34,
    ADDR_W3_DATA_0           = 7'h38,
    ADDR_W3_DATA_1           = 7'h3c,
    ADDR_W3_CTRL             = 7'h40,
    ADDR_W4_DATA_0           = 7'h44,
    ADDR_W4_DATA_1           = 7'h48,
    ADDR_W4_CTRL             = 7'h4c,
    WRIDLE                   = 2'd0,
    WRDATA                   = 2'd1,
    WRRESP                   = 2'd2,
    WRRESET                  = 2'd3,
    RDIDLE                   = 2'd0,
    RDDATA                   = 2'd1,
    RDRESET                  = 2'd2,
    ADDR_BITS                = 7;

//------------------------Local signal-------------------
    reg  [1:0]                    wstate = WRRESET;
    reg  [1:0]                    wnext;
    reg  [ADDR_BITS-1:0]          waddr;
    wire [C_S_AXI_DATA_WIDTH-1:0] wmask;
    wire                          aw_hs;
    wire                          w_hs;
    reg  [1:0]                    rstate = RDRESET;
    reg  [1:0]                    rnext;
    reg  [C_S_AXI_DATA_WIDTH-1:0] rdata;
    wire                          ar_hs;
    wire [ADDR_BITS-1:0]          raddr;
    // internal registers
    reg  [7:0]                    int_mode = 'b0;
    reg  [31:0]                   int_output_scale = 'b0;
    reg  [63:0]                   int_w1 = 'b0;
    reg  [63:0]                   int_w2 = 'b0;
    reg  [63:0]                   int_w3 = 'b0;
    reg  [63:0]                   int_w4 = 'b0;

//------------------------Instantiation------------------


//------------------------AXI write fsm------------------
assign AWREADY = (wstate == WRIDLE);
assign WREADY  = (wstate == WRDATA);
assign BRESP   = 2'b00;  // OKAY
assign BVALID  = (wstate == WRRESP);
assign wmask   = { {8{WSTRB[3]}}, {8{WSTRB[2]}}, {8{WSTRB[1]}}, {8{WSTRB[0]}} };
assign aw_hs   = AWVALID & AWREADY;
assign w_hs    = WVALID & WREADY;

// wstate
always @(posedge ACLK) begin
    if (ARESET)
        wstate <= WRRESET;
    else if (ACLK_EN)
        wstate <= wnext;
end

// wnext
always @(*) begin
    case (wstate)
        WRIDLE:
            if (AWVALID)
                wnext = WRDATA;
            else
                wnext = WRIDLE;
        WRDATA:
            if (WVALID)
                wnext = WRRESP;
            else
                wnext = WRDATA;
        WRRESP:
            if (BREADY)
                wnext = WRIDLE;
            else
                wnext = WRRESP;
        default:
            wnext = WRIDLE;
    endcase
end

// waddr
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (aw_hs)
            waddr <= AWADDR[ADDR_BITS-1:0];
    end
end

//------------------------AXI read fsm-------------------
assign ARREADY = (rstate == RDIDLE);
assign RDATA   = rdata;
assign RRESP   = 2'b00;  // OKAY
assign RVALID  = (rstate == RDDATA);
assign ar_hs   = ARVALID & ARREADY;
assign raddr   = ARADDR[ADDR_BITS-1:0];

// rstate
always @(posedge ACLK) begin
    if (ARESET)
        rstate <= RDRESET;
    else if (ACLK_EN)
        rstate <= rnext;
end

// rnext
always @(*) begin
    case (rstate)
        RDIDLE:
            if (ARVALID)
                rnext = RDDATA;
            else
                rnext = RDIDLE;
        RDDATA:
            if (RREADY & RVALID)
                rnext = RDIDLE;
            else
                rnext = RDDATA;
        default:
            rnext = RDIDLE;
    endcase
end

// rdata
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (ar_hs) begin
            rdata <= 'b0;
            case (raddr)
                ADDR_MODE_DATA_0: begin
                    rdata <= int_mode[7:0];
                end
                ADDR_OUTPUT_SCALE_DATA_0: begin
                    rdata <= int_output_scale[31:0];
                end
                ADDR_W1_DATA_0: begin
                    rdata <= int_w1[31:0];
                end
                ADDR_W1_DATA_1: begin
                    rdata <= int_w1[63:32];
                end
                ADDR_W2_DATA_0: begin
                    rdata <= int_w2[31:0];
                end
                ADDR_W2_DATA_1: begin
                    rdata <= int_w2[63:32];
                end
                ADDR_W3_DATA_0: begin
                    rdata <= int_w3[31:0];
                end
                ADDR_W3_DATA_1: begin
                    rdata <= int_w3[63:32];
                end
                ADDR_W4_DATA_0: begin
                    rdata <= int_w4[31:0];
                end
                ADDR_W4_DATA_1: begin
                    rdata <= int_w4[63:32];
                end
            endcase
        end
    end
end


//------------------------Register logic-----------------
assign mode         = int_mode;
assign output_scale = int_output_scale;
assign w1           = int_w1;
assign w2           = int_w2;
assign w3           = int_w3;
assign w4           = int_w4;
// int_mode[7:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_mode[7:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_MODE_DATA_0)
            int_mode[7:0] <= (WDATA[31:0] & wmask) | (int_mode[7:0] & ~wmask);
    end
end

// int_output_scale[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_output_scale[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_OUTPUT_SCALE_DATA_0)
            int_output_scale[31:0] <= (WDATA[31:0] & wmask) | (int_output_scale[31:0] & ~wmask);
    end
end

// int_w1[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_w1[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W1_DATA_0)
            int_w1[31:0] <= (WDATA[31:0] & wmask) | (int_w1[31:0] & ~wmask);
    end
end

// int_w1[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_w1[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W1_DATA_1)
            int_w1[63:32] <= (WDATA[31:0] & wmask) | (int_w1[63:32] & ~wmask);
    end
end

// int_w2[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_w2[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W2_DATA_0)
            int_w2[31:0] <= (WDATA[31:0] & wmask) | (int_w2[31:0] & ~wmask);
    end
end

// int_w2[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_w2[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W2_DATA_1)
            int_w2[63:32] <= (WDATA[31:0] & wmask) | (int_w2[63:32] & ~wmask);
    end
end

// int_w3[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_w3[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W3_DATA_0)
            int_w3[31:0] <= (WDATA[31:0] & wmask) | (int_w3[31:0] & ~wmask);
    end
end

// int_w3[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_w3[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W3_DATA_1)
            int_w3[63:32] <= (WDATA[31:0] & wmask) | (int_w3[63:32] & ~wmask);
    end
end

// int_w4[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_w4[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W4_DATA_0)
            int_w4[31:0] <= (WDATA[31:0] & wmask) | (int_w4[31:0] & ~wmask);
    end
end

// int_w4[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_w4[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W4_DATA_1)
            int_w4[63:32] <= (WDATA[31:0] & wmask) | (int_w4[63:32] & ~wmask);
    end
end


//------------------------Memory logic-------------------

endmodule
