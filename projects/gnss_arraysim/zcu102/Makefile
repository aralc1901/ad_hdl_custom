####################################################################################
## Copyright (c) 2018 - 2023 Analog Devices, Inc.
### SPDX short identifier: BSD-1-Clause
## Auto-generated, do not modify!
####################################################################################

PROJECT_NAME := gnss_arraysim_zcu102

M_DEPS += ../common/gnss_arraysim_bd.tcl
M_DEPS += ../../scripts/adi_pd.tcl
M_DEPS += ../../common/zcu102/zcu102_system_constr.xdc
M_DEPS += ../../common/zcu102/zcu102_system_bd.tcl
M_DEPS += ../../../library/common/ad_iobuf.v
M_DEPS += ../../../library/axi_ad9361/axi_ad9361_delay.tcl
M_DEPS += ../../../library/gnss_arraysim/gnss_prn_loop/component.xml
M_DEPS += ../../../library/gnss_arraysim/jam_tone_loop/component.xml
M_DEPS += ../../../library/gnss_arraysim/jam_gaussian_loop/component.xml
M_DEPS += ../../../library/gnss_arraysim/fp_arraygen_loop/component.xml
M_DEPS += stream_capture.v

LIB_DEPS += axi_ad9361
LIB_DEPS += axi_dmac
LIB_DEPS += axi_sysid
LIB_DEPS += sysid_rom
LIB_DEPS += util_pack/util_cpack2
LIB_DEPS += util_pack/util_upack2
LIB_DEPS += util_rfifo
LIB_DEPS += util_wfifo
LIB_DEPS += xilinx/util_clkdiv

include ../../scripts/project-xilinx.mk

# Keep the ADI full build available, but make the routine target a BD check in
# a separate directory so it cannot clean a previous bitstream or XSA.
GNSS_FULL_BUILD ?= 0
ifeq ($(GNSS_FULL_BUILD),1)
ifneq ($(origin ADI_SKIP_SYNTHESIS),undefined)
$(error GNSS_FULL_BUILD=1 conflicts with ADI_SKIP_SYNTHESIS)
endif
.DEFAULT_GOAL := all
else ifeq ($(GNSS_FULL_BUILD),0)
.DEFAULT_GOAL := bd
# The generic ADI "all" target deletes generated outputs before running.
# Reject an unflagged explicit "all" before that recipe can start.
$(PROJECT_NAME).sdk/system_top.xsa: gnss_full_required
.PHONY: gnss_full_required
gnss_full_required:
	@echo "Set GNSS_FULL_BUILD=1 in the environment to build the bitstream" >&2; exit 1
else
$(error GNSS_FULL_BUILD must be 0 or 1)
endif

.PHONY: bd
bd: lib
	@mkdir -p gnss_bd_validation
	@ADI_PROJECT_DIR=gnss_bd_validation/ ADI_SKIP_SYNTHESIS=1 \
		vivado -mode batch -source system_project.tcl \
		-log gnss_bd_validation/vivado.log \
		-journal gnss_bd_validation/vivado.jou \
		> gnss_bd_validation/console.log 2>&1
	@echo "Validated gnss_arraysim_zcu102 Block Design (no synthesis or bitstream)"
