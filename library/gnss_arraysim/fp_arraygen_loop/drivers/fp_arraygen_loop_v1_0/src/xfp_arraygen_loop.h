// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XFP_ARRAYGEN_LOOP_H
#define XFP_ARRAYGEN_LOOP_H

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
#include "xfp_arraygen_loop_hw.h"

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
} XFp_arraygen_loop_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XFp_arraygen_loop;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XFp_arraygen_loop_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XFp_arraygen_loop_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XFp_arraygen_loop_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XFp_arraygen_loop_ReadReg(BaseAddress, RegOffset) \
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
int XFp_arraygen_loop_Initialize(XFp_arraygen_loop *InstancePtr, u16 DeviceId);
XFp_arraygen_loop_Config* XFp_arraygen_loop_LookupConfig(u16 DeviceId);
int XFp_arraygen_loop_CfgInitialize(XFp_arraygen_loop *InstancePtr, XFp_arraygen_loop_Config *ConfigPtr);
#else
int XFp_arraygen_loop_Initialize(XFp_arraygen_loop *InstancePtr, const char* InstanceName);
int XFp_arraygen_loop_Release(XFp_arraygen_loop *InstancePtr);
#endif


void XFp_arraygen_loop_Set_mode(XFp_arraygen_loop *InstancePtr, u32 Data);
u32 XFp_arraygen_loop_Get_mode(XFp_arraygen_loop *InstancePtr);
void XFp_arraygen_loop_Set_output_scale(XFp_arraygen_loop *InstancePtr, u32 Data);
u32 XFp_arraygen_loop_Get_output_scale(XFp_arraygen_loop *InstancePtr);
void XFp_arraygen_loop_Set_w1(XFp_arraygen_loop *InstancePtr, u64 Data);
u64 XFp_arraygen_loop_Get_w1(XFp_arraygen_loop *InstancePtr);
void XFp_arraygen_loop_Set_w2(XFp_arraygen_loop *InstancePtr, u64 Data);
u64 XFp_arraygen_loop_Get_w2(XFp_arraygen_loop *InstancePtr);
void XFp_arraygen_loop_Set_w3(XFp_arraygen_loop *InstancePtr, u64 Data);
u64 XFp_arraygen_loop_Get_w3(XFp_arraygen_loop *InstancePtr);
void XFp_arraygen_loop_Set_w4(XFp_arraygen_loop *InstancePtr, u64 Data);
u64 XFp_arraygen_loop_Get_w4(XFp_arraygen_loop *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
