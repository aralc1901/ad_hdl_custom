# Inspect the imported four-antenna ArrayGen package on the ZCU102 target.
source [file normalize [file join [file dirname [info script]] ../../../scripts/adi_env.tcl]]
create_project -in_memory -part xczu9eg-ffvb1156-2-e
set_property ip_repo_paths [list "$ad_hdl_dir/library"] [current_fileset]
update_ip_catalog
set vlnv CTTC:array:fp_arraygen_loop:1.0
if {[llength [get_ipdefs -all -quiet $vlnv]] != 1} {
  error "Missing or ambiguous IP: $vlnv"
}
create_bd_design arraygen_package_check
set arraygen [create_bd_cell -type ip -vlnv $vlnv arraygen]
puts "ARRAYGEN_PACKAGE_TARGET [get_property PART [current_project]]"
puts "ARRAYGEN_PACKAGE_VLNV [get_property VLNV $arraygen]"
puts "ARRAYGEN_PACKAGE_INTERFACE [get_bd_intf_pins $arraygen/*]"
puts "ARRAYGEN_PACKAGE_PINS [get_bd_pins $arraygen/*]"
close_project
