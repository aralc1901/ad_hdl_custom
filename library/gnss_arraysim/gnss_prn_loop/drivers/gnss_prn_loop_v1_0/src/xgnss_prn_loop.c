// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xgnss_prn_loop.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XGnss_prn_loop_CfgInitialize(XGnss_prn_loop *InstancePtr, XGnss_prn_loop_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XGnss_prn_loop_Start(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_AP_CTRL) & 0x80;
    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XGnss_prn_loop_IsDone(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XGnss_prn_loop_IsIdle(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XGnss_prn_loop_IsReady(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XGnss_prn_loop_EnableAutoRestart(XGnss_prn_loop *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XGnss_prn_loop_DisableAutoRestart(XGnss_prn_loop *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_AP_CTRL, 0);
}

void XGnss_prn_loop_Set_n_samples(XGnss_prn_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_N_SAMPLES_DATA, Data);
}

u32 XGnss_prn_loop_Get_n_samples(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_N_SAMPLES_DATA);
    return Data;
}

void XGnss_prn_loop_Set_prn_length(XGnss_prn_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_LENGTH_DATA, Data);
}

u32 XGnss_prn_loop_Get_prn_length(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_LENGTH_DATA);
    return Data;
}

void XGnss_prn_loop_Set_sample_step(XGnss_prn_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_SAMPLE_STEP_DATA, Data);
}

u32 XGnss_prn_loop_Get_sample_step(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_SAMPLE_STEP_DATA);
    return Data;
}

void XGnss_prn_loop_Set_prn_phase_init(XGnss_prn_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_PHASE_INIT_DATA, Data);
}

u32 XGnss_prn_loop_Get_prn_phase_init(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_PHASE_INIT_DATA);
    return Data;
}

void XGnss_prn_loop_Set_doppler_freq(XGnss_prn_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_DOPPLER_FREQ_DATA, Data);
}

u32 XGnss_prn_loop_Get_doppler_freq(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_DOPPLER_FREQ_DATA);
    return Data;
}

void XGnss_prn_loop_Set_carrier_phase_init(XGnss_prn_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_CARRIER_PHASE_INIT_DATA, Data);
}

u32 XGnss_prn_loop_Get_carrier_phase_init(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_CARRIER_PHASE_INIT_DATA);
    return Data;
}

void XGnss_prn_loop_Set_fs(XGnss_prn_loop *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_FS_DATA, Data);
}

u32 XGnss_prn_loop_Get_fs(XGnss_prn_loop *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_FS_DATA);
    return Data;
}

u32 XGnss_prn_loop_Get_prn_mem_BaseAddress(XGnss_prn_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return (InstancePtr->Control_BaseAddress + XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE);
}

u32 XGnss_prn_loop_Get_prn_mem_HighAddress(XGnss_prn_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return (InstancePtr->Control_BaseAddress + XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_HIGH);
}

u32 XGnss_prn_loop_Get_prn_mem_TotalBytes(XGnss_prn_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return (XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_HIGH - XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + 1);
}

u32 XGnss_prn_loop_Get_prn_mem_BitWidth(XGnss_prn_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XGNSS_PRN_LOOP_CONTROL_WIDTH_PRN_MEM;
}

u32 XGnss_prn_loop_Get_prn_mem_Depth(XGnss_prn_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XGNSS_PRN_LOOP_CONTROL_DEPTH_PRN_MEM;
}

u32 XGnss_prn_loop_Write_prn_mem_Words(XGnss_prn_loop *InstancePtr, int offset, word_type *data, int length) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr -> IsReady == XIL_COMPONENT_IS_READY);

    int i;

    if ((offset + length)*4 > (XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_HIGH - XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + 1))
        return 0;

    for (i = 0; i < length; i++) {
        *(int *)(InstancePtr->Control_BaseAddress + XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + (offset + i)*4) = *(data + i);
    }
    return length;
}

u32 XGnss_prn_loop_Read_prn_mem_Words(XGnss_prn_loop *InstancePtr, int offset, word_type *data, int length) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr -> IsReady == XIL_COMPONENT_IS_READY);

    int i;

    if ((offset + length)*4 > (XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_HIGH - XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + 1))
        return 0;

    for (i = 0; i < length; i++) {
        *(data + i) = *(int *)(InstancePtr->Control_BaseAddress + XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + (offset + i)*4);
    }
    return length;
}

u32 XGnss_prn_loop_Write_prn_mem_Bytes(XGnss_prn_loop *InstancePtr, int offset, char *data, int length) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr -> IsReady == XIL_COMPONENT_IS_READY);

    int i;

    if ((offset + length) > (XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_HIGH - XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + 1))
        return 0;

    for (i = 0; i < length; i++) {
        *(char *)(InstancePtr->Control_BaseAddress + XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + offset + i) = *(data + i);
    }
    return length;
}

u32 XGnss_prn_loop_Read_prn_mem_Bytes(XGnss_prn_loop *InstancePtr, int offset, char *data, int length) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr -> IsReady == XIL_COMPONENT_IS_READY);

    int i;

    if ((offset + length) > (XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_HIGH - XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + 1))
        return 0;

    for (i = 0; i < length; i++) {
        *(data + i) = *(char *)(InstancePtr->Control_BaseAddress + XGNSS_PRN_LOOP_CONTROL_ADDR_PRN_MEM_BASE + offset + i);
    }
    return length;
}

void XGnss_prn_loop_InterruptGlobalEnable(XGnss_prn_loop *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_GIE, 1);
}

void XGnss_prn_loop_InterruptGlobalDisable(XGnss_prn_loop *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_GIE, 0);
}

void XGnss_prn_loop_InterruptEnable(XGnss_prn_loop *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_IER);
    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_IER, Register | Mask);
}

void XGnss_prn_loop_InterruptDisable(XGnss_prn_loop *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_IER);
    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_IER, Register & (~Mask));
}

void XGnss_prn_loop_InterruptClear(XGnss_prn_loop *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGnss_prn_loop_WriteReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_ISR, Mask);
}

u32 XGnss_prn_loop_InterruptGetEnabled(XGnss_prn_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_IER);
}

u32 XGnss_prn_loop_InterruptGetStatus(XGnss_prn_loop *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XGnss_prn_loop_ReadReg(InstancePtr->Control_BaseAddress, XGNSS_PRN_LOOP_CONTROL_ADDR_ISR);
}

