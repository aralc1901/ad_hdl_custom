// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xjam_gaussian_loop.h"

extern XJam_gaussian_loop_Config XJam_gaussian_loop_ConfigTable[];

XJam_gaussian_loop_Config *XJam_gaussian_loop_LookupConfig(u16 DeviceId) {
	XJam_gaussian_loop_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XJAM_GAUSSIAN_LOOP_NUM_INSTANCES; Index++) {
		if (XJam_gaussian_loop_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XJam_gaussian_loop_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XJam_gaussian_loop_Initialize(XJam_gaussian_loop *InstancePtr, u16 DeviceId) {
	XJam_gaussian_loop_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XJam_gaussian_loop_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XJam_gaussian_loop_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

