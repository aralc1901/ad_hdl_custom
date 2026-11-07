// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
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

#define XFP_ARRAYGEN_LOOP_CONTROL_ADDR_MODE_DATA         0x10
#define XFP_ARRAYGEN_LOOP_CONTROL_BITS_MODE_DATA         8
#define XFP_ARRAYGEN_LOOP_CONTROL_ADDR_OUTPUT_SCALE_DATA 0x18
#define XFP_ARRAYGEN_LOOP_CONTROL_BITS_OUTPUT_SCALE_DATA 32
#define XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W1_DATA           0x20
#define XFP_ARRAYGEN_LOOP_CONTROL_BITS_W1_DATA           64
#define XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W2_DATA           0x2c
#define XFP_ARRAYGEN_LOOP_CONTROL_BITS_W2_DATA           64
#define XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W3_DATA           0x38
#define XFP_ARRAYGEN_LOOP_CONTROL_BITS_W3_DATA           64
#define XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W4_DATA           0x44
#define XFP_ARRAYGEN_LOOP_CONTROL_BITS_W4_DATA           64

