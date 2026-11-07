// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XJAM_TONE_LOOP_H
#define XJAM_TONE_LOOP_H

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
#include "xjam_tone_loop_hw.h"

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
} XJam_tone_loop_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XJam_tone_loop;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XJam_tone_loop_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XJam_tone_loop_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XJam_tone_loop_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XJam_tone_loop_ReadReg(BaseAddress, RegOffset) \
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
int XJam_tone_loop_Initialize(XJam_tone_loop *InstancePtr, u16 DeviceId);
XJam_tone_loop_Config* XJam_tone_loop_LookupConfig(u16 DeviceId);
int XJam_tone_loop_CfgInitialize(XJam_tone_loop *InstancePtr, XJam_tone_loop_Config *ConfigPtr);
#else
int XJam_tone_loop_Initialize(XJam_tone_loop *InstancePtr, const char* InstanceName);
int XJam_tone_loop_Release(XJam_tone_loop *InstancePtr);
#endif

void XJam_tone_loop_Start(XJam_tone_loop *InstancePtr);
u32 XJam_tone_loop_IsDone(XJam_tone_loop *InstancePtr);
u32 XJam_tone_loop_IsIdle(XJam_tone_loop *InstancePtr);
u32 XJam_tone_loop_IsReady(XJam_tone_loop *InstancePtr);
void XJam_tone_loop_EnableAutoRestart(XJam_tone_loop *InstancePtr);
void XJam_tone_loop_DisableAutoRestart(XJam_tone_loop *InstancePtr);

void XJam_tone_loop_Set_n_samples(XJam_tone_loop *InstancePtr, u32 Data);
u32 XJam_tone_loop_Get_n_samples(XJam_tone_loop *InstancePtr);
void XJam_tone_loop_Set_amplitude(XJam_tone_loop *InstancePtr, u32 Data);
u32 XJam_tone_loop_Get_amplitude(XJam_tone_loop *InstancePtr);
void XJam_tone_loop_Set_phase_step(XJam_tone_loop *InstancePtr, u32 Data);
u32 XJam_tone_loop_Get_phase_step(XJam_tone_loop *InstancePtr);

void XJam_tone_loop_InterruptGlobalEnable(XJam_tone_loop *InstancePtr);
void XJam_tone_loop_InterruptGlobalDisable(XJam_tone_loop *InstancePtr);
void XJam_tone_loop_InterruptEnable(XJam_tone_loop *InstancePtr, u32 Mask);
void XJam_tone_loop_InterruptDisable(XJam_tone_loop *InstancePtr, u32 Mask);
void XJam_tone_loop_InterruptClear(XJam_tone_loop *InstancePtr, u32 Mask);
u32 XJam_tone_loop_InterruptGetEnabled(XJam_tone_loop *InstancePtr);
u32 XJam_tone_loop_InterruptGetStatus(XJam_tone_loop *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
