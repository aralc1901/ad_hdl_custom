// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xjam_gaussian_loop.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XJam_gaussian_loop_CfgInitialize(XJam_gaussian_loop *InstancePtr, XJam_gaussian_loop_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XJam_gaussian_loop_Start(XJam_gaussian_loop *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AP_CTRL) & 0x80;
    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XJam_gaussian_loop_IsDone(XJam_gaussian_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XJam_gaussian_loop_IsIdle(XJam_gaussian_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XJam_gaussian_loop_IsReady(XJam_gaussian_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XJam_gaussian_loop_EnableAutoRestart(XJam_gaussian_loop *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XJam_gaussian_loop_DisableAutoRestart(XJam_gaussian_loop *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AP_CTRL, 0);
}

void XJam_gaussian_loop_Set_n_samples(XJam_gaussian_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_N_SAMPLES_DATA, Data);
}

u32 XJam_gaussian_loop_Get_n_samples(XJam_gaussian_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_N_SAMPLES_DATA);
    return Data;
}

void XJam_gaussian_loop_Set_amplitude(XJam_gaussian_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AMPLITUDE_DATA, Data);
}

u32 XJam_gaussian_loop_Get_amplitude(XJam_gaussian_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_AMPLITUDE_DATA);
    return Data;
}

void XJam_gaussian_loop_Set_seed(XJam_gaussian_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_SEED_DATA, Data);
}

u32 XJam_gaussian_loop_Get_seed(XJam_gaussian_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_SEED_DATA);
    return Data;
}

void XJam_gaussian_loop_InterruptGlobalEnable(XJam_gaussian_loop *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_GIE, 1);
}

void XJam_gaussian_loop_InterruptGlobalDisable(XJam_gaussian_loop *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_GIE, 0);
}

void XJam_gaussian_loop_InterruptEnable(XJam_gaussian_loop *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_IER);
    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_IER, Register | Mask);
}

void XJam_gaussian_loop_InterruptDisable(XJam_gaussian_loop *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_IER);
    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_IER, Register & (~Mask));
}

void XJam_gaussian_loop_InterruptClear(XJam_gaussian_loop *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XJam_gaussian_loop_WriteReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_ISR, Mask);
}

u32 XJam_gaussian_loop_InterruptGetEnabled(XJam_gaussian_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_IER);
}

u32 XJam_gaussian_loop_InterruptGetStatus(XJam_gaussian_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XJam_gaussian_loop_ReadReg(InstancePtr->Control_BaseAddress, XJAM_GAUSSIAN_LOOP_CONTROL_ADDR_ISR);
}

