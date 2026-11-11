source ../../../scripts/adi_env.tcl
source $ad_hdl_dir/projects/scripts/adi_project_xilinx.tcl
source $ad_hdl_dir/projects/scripts/adi_board.tcl

# The ADI runner checks whether ADI_SKIP_SYNTHESIS exists, independent of its
# value. Reject conflicting flags before creating any Vivado project files.
set gnss_full_build 0
if {[info exists ::env(GNSS_FULL_BUILD)]} {
  if {$::env(GNSS_FULL_BUILD) ni {0 1}} {
    error "GNSS_FULL_BUILD must be 0 or 1"
  }
  set gnss_full_build [expr {$::env(GNSS_FULL_BUILD) eq "1"}]
}
if {$gnss_full_build && [info exists ::env(ADI_SKIP_SYNTHESIS)]} {
  error "GNSS_FULL_BUILD=1 conflicts with ADI_SKIP_SYNTHESIS"
}

adi_project gnss_arraysim_zcu102

# ADI registers $ad_hdl_dir/library before sourcing system_bd.tcl. The GNSS
# packages are below that root; keep the requirement explicit in this project.
set gnss_ip_repo [file normalize "$ad_hdl_dir/library/gnss_arraysim"]
if {![file isdirectory $gnss_ip_repo]} {
  error "Missing GNSS IP repository: $gnss_ip_repo"
}
set ip_repos [get_property ip_repo_paths [current_fileset]]
set library_repo [file normalize "$ad_hdl_dir/library"]
if {[lsearch -exact $ip_repos $library_repo] < 0} {
  lappend ip_repos $library_repo
}
set_property ip_repo_paths $ip_repos [current_fileset]
update_ip_catalog

adi_project_files gnss_arraysim_zcu102 [list \
  "system_top.v" \
  "system_constr.xdc"\
  "$ad_hdl_dir/library/common/ad_iobuf.v" \
  "$ad_hdl_dir/projects/common/zcu102/zcu102_system_constr.xdc" ]

# Direct Vivado invocations follow the same opt-in policy as make.
if {!$gnss_full_build} {
  set ::env(ADI_SKIP_SYNTHESIS) 1
}

adi_project_run gnss_arraysim_zcu102
if {$gnss_full_build} {
  source $ad_hdl_dir/library/axi_ad9361/axi_ad9361_delay.tcl
}
