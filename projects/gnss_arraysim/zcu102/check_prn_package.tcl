# Inspect the imported PRN package on the ZCU102 part without synthesis.
source [file normalize [file join [file dirname [info script]] ../../../scripts/adi_env.tcl]]
create_project -in_memory -part xczu9eg-ffvb1156-2-e
set_property ip_repo_paths [list "$ad_hdl_dir/library"] [current_fileset]
update_ip_catalog
set vlnv xilinx.com:hls:gnss_prn_loop:1.0
if {[llength [get_ipdefs -all -quiet $vlnv]] != 1} {
  error "Missing or ambiguous IP: $vlnv"
}
create_bd_design prn_package_check
set prn [create_bd_cell -type ip -vlnv $vlnv prn]
puts "PRN_PACKAGE_TARGET [get_property PART [current_project]]"
puts "PRN_PACKAGE_VLNV [get_property VLNV $prn]"
puts "PRN_PACKAGE_INTERFACE [get_bd_intf_pins $prn/*]"
puts "PRN_PACKAGE_PINS [get_bd_pins $prn/*]"
close_project
