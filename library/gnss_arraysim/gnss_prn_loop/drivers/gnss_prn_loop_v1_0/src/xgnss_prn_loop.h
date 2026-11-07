// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XGNSS_PRN_LOOP_H
#define XGNSS_PRN_LOOP_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xgnss_prn_loop_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u64 Control_BaseAddress;
} XGnss_prn_loop_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XGnss_prn_loop;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XGnss_prn_loop_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XGnss_prn_loop_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XGnss_prn_loop_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XGnss_prn_loop_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XGnss_prn_loop_Initialize(XGnss_prn_loop *InstancePtr, u16 DeviceId);
XGnss_prn_loop_Config* XGnss_prn_loop_LookupConfig(u16 DeviceId);
int XGnss_prn_loop_CfgInitialize(XGnss_prn_loop *InstancePtr, XGnss_prn_loop_Config *ConfigPtr);
#else
int XGnss_prn_loop_Initialize(XGnss_prn_loop *InstancePtr, const char* InstanceName);
int XGnss_prn_loop_Release(XGnss_prn_loop *InstancePtr);
#endif

void XGnss_prn_loop_Start(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_IsDone(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_IsIdle(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_IsReady(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_EnableAutoRestart(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_DisableAutoRestart(XGnss_prn_loop *InstancePtr);

void XGnss_prn_loop_Set_n_samples(XGnss_prn_loop *InstancePtr, u32 Data);
u32 XGnss_prn_loop_Get_n_samples(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_Set_prn_length(XGnss_prn_loop *InstancePtr, u32 Data);
u32 XGnss_prn_loop_Get_prn_length(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_Set_sample_step(XGnss_prn_loop *InstancePtr, u32 Data);
u32 XGnss_prn_loop_Get_sample_step(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_Set_prn_phase_init(XGnss_prn_loop *InstancePtr, u32 Data);
u32 XGnss_prn_loop_Get_prn_phase_init(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_Set_doppler_freq(XGnss_prn_loop *InstancePtr, u32 Data);
u32 XGnss_prn_loop_Get_doppler_freq(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_Set_carrier_phase_init(XGnss_prn_loop *InstancePtr, u32 Data);
u32 XGnss_prn_loop_Get_carrier_phase_init(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_Set_fs(XGnss_prn_loop *InstancePtr, u32 Data);
u32 XGnss_prn_loop_Get_fs(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_Get_prn_mem_BaseAddress(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_Get_prn_mem_HighAddress(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_Get_prn_mem_TotalBytes(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_Get_prn_mem_BitWidth(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_Get_prn_mem_Depth(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_Write_prn_mem_Words(XGnss_prn_loop *InstancePtr, int offset, word_type *data, int length);
u32 XGnss_prn_loop_Read_prn_mem_Words(XGnss_prn_loop *InstancePtr, int offset, word_type *data, int length);
u32 XGnss_prn_loop_Write_prn_mem_Bytes(XGnss_prn_loop *InstancePtr, int offset, char *data, int length);
u32 XGnss_prn_loop_Read_prn_mem_Bytes(XGnss_prn_loop *InstancePtr, int offset, char *data, int length);

void XGnss_prn_loop_InterruptGlobalEnable(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_InterruptGlobalDisable(XGnss_prn_loop *InstancePtr);
void XGnss_prn_loop_InterruptEnable(XGnss_prn_loop *InstancePtr, u32 Mask);
void XGnss_prn_loop_InterruptDisable(XGnss_prn_loop *InstancePtr, u32 Mask);
void XGnss_prn_loop_InterruptClear(XGnss_prn_loop *InstancePtr, u32 Mask);
u32 XGnss_prn_loop_InterruptGetEnabled(XGnss_prn_loop *InstancePtr);
u32 XGnss_prn_loop_InterruptGetStatus(XGnss_prn_loop *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
