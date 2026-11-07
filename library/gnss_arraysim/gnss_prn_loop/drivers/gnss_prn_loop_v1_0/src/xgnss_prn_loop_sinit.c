// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xgnss_prn_loop.h"

extern XGnss_prn_loop_Config XGnss_prn_loop_ConfigTable[];

XGnss_prn_loop_Config *XGnss_prn_loop_LookupConfig(u16 DeviceId) {
	XGnss_prn_loop_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XGNSS_PRN_LOOP_NUM_INSTANCES; Index++) {
		if (XGnss_prn_loop_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XGnss_prn_loop_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XGnss_prn_loop_Initialize(XGnss_prn_loop *InstancePtr, u16 DeviceId) {
	XGnss_prn_loop_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XGnss_prn_loop_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XGnss_prn_loop_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

