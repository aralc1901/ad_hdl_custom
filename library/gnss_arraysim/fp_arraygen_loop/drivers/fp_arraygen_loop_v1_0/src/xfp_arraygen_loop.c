// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xfp_arraygen_loop.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XFp_arraygen_loop_CfgInitialize(XFp_arraygen_loop *InstancePtr, XFp_arraygen_loop_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XFp_arraygen_loop_Set_mode(XFp_arraygen_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_MODE_DATA, Data);
}

u32 XFp_arraygen_loop_Get_mode(XFp_arraygen_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_MODE_DATA);
    return Data;
}

void XFp_arraygen_loop_Set_output_scale(XFp_arraygen_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_OUTPUT_SCALE_DATA, Data);
}

u32 XFp_arraygen_loop_Get_output_scale(XFp_arraygen_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_OUTPUT_SCALE_DATA);
    return Data;
}

void XFp_arraygen_loop_Set_w1(XFp_arraygen_loop *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W1_DATA, (u32)(Data));
    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W1_DATA + 4, (u32)(Data >> 32));
}

u64 XFp_arraygen_loop_Get_w1(XFp_arraygen_loop *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W1_DATA);
    Data += (u64)XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W1_DATA + 4) << 32;
    return Data;
}

void XFp_arraygen_loop_Set_w2(XFp_arraygen_loop *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W2_DATA, (u32)(Data));
    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W2_DATA + 4, (u32)(Data >> 32));
}

u64 XFp_arraygen_loop_Get_w2(XFp_arraygen_loop *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W2_DATA);
    Data += (u64)XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W2_DATA + 4) << 32;
    return Data;
}

void XFp_arraygen_loop_Set_w3(XFp_arraygen_loop *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W3_DATA, (u32)(Data));
    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W3_DATA + 4, (u32)(Data >> 32));
}

u64 XFp_arraygen_loop_Get_w3(XFp_arraygen_loop *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W3_DATA);
    Data += (u64)XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W3_DATA + 4) << 32;
    return Data;
}

void XFp_arraygen_loop_Set_w4(XFp_arraygen_loop *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W4_DATA, (u32)(Data));
    XFp_arraygen_loop_WriteReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W4_DATA + 4, (u32)(Data >> 32));
}

u64 XFp_arraygen_loop_Get_w4(XFp_arraygen_loop *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W4_DATA);
    Data += (u64)XFp_arraygen_loop_ReadReg(InstancePtr->Control_BaseAddress, XFP_ARRAYGEN_LOOP_CONTROL_ADDR_W4_DATA + 4) << 32;
    return Data;
}

