# Run from this directory with Vivado 2022.2, without synthesis or a license.
source ../../../scripts/adi_env.tcl
create_project -in_memory -part xczu9eg-ffvb1156-2-e
set_property ip_repo_paths [list "$ad_hdl_dir/library"] [current_fileset]
update_ip_catalog

foreach ipdef {
  xilinx.com:hls:gnss_prn_loop:1.0
  xilinx.com:hls:jam_tone_loop:1.0
  xilinx.com:hls:jam_gaussian_loop:1.0
  CTTC:array:fp_arraygen_loop:1.0
} {
  if {[llength [get_ipdefs -all -quiet $ipdef]] != 1} {
    error "IP catalog lookup failed for $ipdef"
  }
  puts "GNSS_IP_FOUND $ipdef"
}

close_project
