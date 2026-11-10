# Inspect the exact imported package on the target part without synthesis.
source [file normalize [file join [file dirname [info script]] ../../../scripts/adi_env.tcl]]
create_project -in_memory -part xczu9eg-ffvb1156-2-e
set_property ip_repo_paths [list "$ad_hdl_dir/library"] [current_fileset]
update_ip_catalog
set vlnv xilinx.com:hls:jam_tone_loop:1.0
if {[llength [get_ipdefs -all -quiet $vlnv]] != 1} {
  error "Missing or ambiguous IP: $vlnv"
}
create_bd_design tone_package_check
set tone [create_bd_cell -type ip -vlnv $vlnv tone]
puts "TONE_PACKAGE_TARGET [get_property PART [current_project]]"
puts "TONE_PACKAGE_VLNV [get_property VLNV $tone]"
puts "TONE_PACKAGE_INTERFACE [get_bd_intf_pins $tone/*]"
puts "TONE_PACKAGE_PINS [get_bd_pins $tone/*]"
close_project
