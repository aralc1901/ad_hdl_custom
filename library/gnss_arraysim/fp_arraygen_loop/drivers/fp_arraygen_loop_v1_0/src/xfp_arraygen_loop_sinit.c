// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xfp_arraygen_loop.h"

extern XFp_arraygen_loop_Config XFp_arraygen_loop_ConfigTable[];

XFp_arraygen_loop_Config *XFp_arraygen_loop_LookupConfig(u16 DeviceId) {
	XFp_arraygen_loop_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XFP_ARRAYGEN_LOOP_NUM_INSTANCES; Index++) {
		if (XFp_arraygen_loop_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XFp_arraygen_loop_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XFp_arraygen_loop_Initialize(XFp_arraygen_loop *InstancePtr, u16 DeviceId) {
	XFp_arraygen_loop_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XFp_arraygen_loop_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XFp_arraygen_loop_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

