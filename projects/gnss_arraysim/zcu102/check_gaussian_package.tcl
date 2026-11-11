# Inspect the imported Gaussian package on the ZCU102 part without synthesis.
source [file normalize [file join [file dirname [info script]] ../../../scripts/adi_env.tcl]]
create_project -in_memory -part xczu9eg-ffvb1156-2-e
set_property ip_repo_paths [list "$ad_hdl_dir/library"] [current_fileset]
update_ip_catalog
set vlnv xilinx.com:hls:jam_gaussian_loop:1.0
if {[llength [get_ipdefs -all -quiet $vlnv]] != 1} {
  error "Missing or ambiguous IP: $vlnv"
}
create_bd_design gaussian_package_check
set gaussian [create_bd_cell -type ip -vlnv $vlnv gaussian]
puts "GAUSSIAN_PACKAGE_TARGET [get_property PART [current_project]]"
puts "GAUSSIAN_PACKAGE_VLNV [get_property VLNV $gaussian]"
puts "GAUSSIAN_PACKAGE_INTERFACE [get_bd_intf_pins $gaussian/*]"
puts "GAUSSIAN_PACKAGE_PINS [get_bd_pins $gaussian/*]"
close_project
