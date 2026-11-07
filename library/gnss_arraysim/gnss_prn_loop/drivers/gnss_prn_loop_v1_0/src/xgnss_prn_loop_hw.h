// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x0000 : Control signals
//          bit 0  - ap_start (Read/Write/COH)
//          bit 1  - ap_done (Read/COR)
//          bit 2  - ap_idle (Read)
//          bit 3  - ap_ready (Read/COR)
//          bit 7  - auto_restart (Read/Write)
//          bit 9  - interrupt (Read)
//          others - reserved
// 0x0004 : Global Interrupt Enable Register
//          bit 0  - Global Interrupt Enable (Read/Write)
//          others - reserved
// 0x0008 : IP Interrupt Enable Register (Read/Write)
//          bit 0 - enable ap_done interrupt (Read/Write)
//          bit 1 - enable ap_ready interrupt (Read/Write)
//          others - reserved
// 0x000c : IP Interrupt Status Register (Read/TOW)
//          bit 0 - ap_done (Read/TOW)
//          bit 1 - ap_ready (Read/TOW)
//          others - reserved
// 0x0010 : Data signal of n_samples
//          bit 31~0 - n_samples[31:0] (Read/Write)
// 0x0014 : reserved
// 0x0018 : Data signal of prn_length
//          bit 31~0 - prn_length[31:0] (Read/Write)
// 0x001c : reserved
// 0x0020 : Data signal of sample_step
//          bit 31~0 - sample_step[31:0] (Read/Write)
// 0x0024 : reserved
// 0x0028 : Data signal of prn_phase_init
//          bit 31~0 - prn_phase_init[31:0] (Read/Write)
// 0x002c : reserved
// 0x0030 : Data signal of doppler_freq
//          bit 31~0 - doppler_freq[31:0] (Read/Write)
// 0x0034 : reserved
// 0x0038 : Data signal of carrier_phase_init
//          bit 31~0 - carrier_phase_init[31:0] (Read/Write)
// 0x003c : reserved
// 0x0040 : Data signal of fs
//          bit 31~0 - fs[31:0] (Read/Write)
// 0x0044 : reserved
// 0x2000 ~
// 0x3fff : Memory 'prn_mem' (4096 * 16b)
//          Word n : bit [15: 0] - prn_mem[2n]
//                   bit [31:16] - prn_mem[2n+1]
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XGNSS_PRN_LOOP_CONTROL_ADDR_AP_CTRL                 0x0000
#define XGNSS_PRN_LOOP_CONTROL_ADDR_GIE                     0x0004
#define XGNSS_PRN_LOOP_CONTROL_ADDR_IER                     0x0008
#define XGNSS_PRN_LOOP_CONTROL_ADDR_ISR                     0x000c
#define XGNSS_PRN_LOOP_CONTROL_ADDR_N_SAMPLES_DATA          0x0010
#define XGNSS_PRN_LOOP_CONTROL_BITS_N_SAMPLES_DATA          32
#define XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_LENGTH_DATA         0x0018
#define XGNSS_PRN_LOOP_CONTROL_BITS_PRN_LENGTH_DATA         32
#define XGNSS_PRN_LOOP_CONTROL_ADDR_SAMPLE_STEP_DATA        0x0020
#define XGNSS_PRN_LOOP_CONTROL_BITS_SAMPLE_STEP_DATA        32
#define XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_PHASE_INIT_DATA     0x0028
#define XGNSS_PRN_LOOP_CONTROL_BITS_PRN_PHASE_INIT_DATA     32
#define XGNSS_PRN_LOOP_CONTROL_ADDR_DOPPLER_FREQ_DATA       0x0030
#define XGNSS_PRN_LOOP_CONTROL_BITS_DOPPLER_FREQ_DATA       32
#define XGNSS_PRN_LOOP_CONTROL_ADDR_CARRIER_PHASE_INIT_DATA 0x0038
#define XGNSS_PRN_LOOP_CONTROL_BITS_CARRIER_PHASE_INIT_DATA 32
#define XGNSS_PRN_LOOP_CONTROL_ADDR_FS_DATA                 0x0040
#define XGNSS_PRN_LOOP_CONTROL_BITS_FS_DATA                 32
#define XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE            0x2000
#define XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_HIGH            0x3fff
#define XGNSS_PRN_LOOP_CONTROL_WIDTH_PRN_MEM                16
#define XGNSS_PRN_LOOP_CONTROL_DEPTH_PRN_MEM                4096

