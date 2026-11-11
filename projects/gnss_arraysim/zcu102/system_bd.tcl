source $ad_hdl_dir/projects/common/zcu102/zcu102_system_bd.tcl
source ../common/gnss_arraysim_bd.tcl
source $ad_hdl_dir/projects/scripts/adi_pd.tcl

set mem_init_sys_path [get_env_param ADI_PROJECT_DIR ""]mem_init_sys.txt;

#system ID
ad_ip_parameter axi_sysid_0 CONFIG.ROM_ADDR_BITS 9
ad_ip_parameter rom_sys_0 CONFIG.PATH_TO_FILE "[pwd]/$mem_init_sys_path"
ad_ip_parameter rom_sys_0 CONFIG.ROM_ADDR_BITS 9

sysid_gen_sys_init_file

ad_ip_parameter axi_ad9361_0 CONFIG.ADC_INIT_DELAY 8
ad_ip_parameter axi_ad9361_0 CONFIG.DELAY_REFCLK_FREQUENCY 500
ad_ip_parameter axi_ad9361_1 CONFIG.ADC_INIT_DELAY 8
ad_ip_parameter axi_ad9361_1 CONFIG.DELAY_REFCLK_FREQUENCY 500

ad_ip_parameter util_ad9361_divclk CONFIG.SIM_DEVICE ULTRASCALE

# Independent digital tone capture. No FMCOMMS5 receive or transmit net is
# changed; the existing ADC and DAC DMA connections remain in the common BD.
set stream_capture_rtl "$ad_hdl_dir/projects/gnss_arraysim/zcu102/stream_capture.v"
add_files -norecurse -fileset sources_1 $stream_capture_rtl
update_compile_order -fileset sources_1

create_bd_cell -type ip -vlnv xilinx.com:hls:jam_tone_loop:1.0 jam_tone_0
ad_connect $sys_cpu_clk jam_tone_0/ap_clk
ad_connect $sys_cpu_resetn jam_tone_0/ap_rst_n
ad_cpu_interconnect 0x47000000 jam_tone_0
ad_cpu_interrupt ps-14 mb-14 jam_tone_0/interrupt

create_bd_cell -type module -reference stream_capture tone_capture_0
ad_connect $sys_cpu_clk tone_capture_0/clk
ad_connect $sys_cpu_resetn tone_capture_0/resetn
ad_connect jam_tone_0/tone_out_TVALID tone_capture_0/sample_valid
ad_connect tone_capture_0/sample_ready jam_tone_0/tone_out_TREADY

# GPIO channel 1: arm, pause and length. Channel 2: read-only capture status.
ad_ip_instance axi_gpio tone_capture_gpio
ad_ip_parameter tone_capture_gpio CONFIG.C_IS_DUAL 1
ad_ip_parameter tone_capture_gpio CONFIG.C_GPIO_WIDTH 32
ad_ip_parameter tone_capture_gpio CONFIG.C_GPIO2_WIDTH 32
ad_ip_parameter tone_capture_gpio CONFIG.C_ALL_OUTPUTS 1
ad_ip_parameter tone_capture_gpio CONFIG.C_ALL_INPUTS_2 1
ad_connect tone_capture_gpio/gpio_io_o tone_capture_0/control
ad_connect tone_capture_0/status tone_capture_gpio/gpio2_io_i
ad_cpu_interconnect 0x47020000 tone_capture_gpio

# PS reads port A through AXI BRAM Controller; the capture writes port B.
ad_ip_instance axi_bram_ctrl tone_capture_axi_bram
ad_ip_parameter tone_capture_axi_bram CONFIG.SINGLE_PORT_BRAM 1
ad_ip_instance blk_mem_gen tone_capture_bram
ad_ip_parameter tone_capture_bram CONFIG.Memory_Type {True_Dual_Port_RAM}
ad_ip_parameter tone_capture_bram CONFIG.Enable_32bit_Address false
ad_ip_parameter tone_capture_bram CONFIG.Write_Width_A 32
ad_ip_parameter tone_capture_bram CONFIG.Read_Width_A 32
ad_ip_parameter tone_capture_bram CONFIG.Write_Width_B 32
ad_ip_parameter tone_capture_bram CONFIG.Read_Width_B 32
ad_ip_parameter tone_capture_bram CONFIG.Write_Depth_A 1024
ad_connect tone_capture_axi_bram/BRAM_PORTA tone_capture_bram/BRAM_PORTA
ad_connect $sys_cpu_clk tone_capture_bram/clkb
ad_connect tone_capture_0/capture_write_en tone_capture_bram/enb
ad_connect tone_capture_0/capture_write_en tone_capture_bram/web
ad_connect tone_capture_0/capture_write_addr tone_capture_bram/addrb
ad_connect jam_tone_0/tone_out_TDATA tone_capture_bram/dinb
ad_cpu_interconnect 0x47010000 tone_capture_axi_bram

# Independent Gaussian noise capture. The same handshake-counted helper is
# instantiated again; each generator has separate PS control, GPIO and BRAM.
create_bd_cell -type ip -vlnv xilinx.com:hls:jam_gaussian_loop:1.0 jam_gaussian_0
ad_connect $sys_cpu_clk jam_gaussian_0/ap_clk
ad_connect $sys_cpu_resetn jam_gaussian_0/ap_rst_n
ad_cpu_interconnect 0x47030000 jam_gaussian_0
ad_cpu_interrupt ps-15 mb-15 jam_gaussian_0/interrupt

create_bd_cell -type module -reference stream_capture gaussian_capture_0
ad_connect $sys_cpu_clk gaussian_capture_0/clk
ad_connect $sys_cpu_resetn gaussian_capture_0/resetn
ad_connect jam_gaussian_0/noise_out_TVALID gaussian_capture_0/sample_valid
ad_connect gaussian_capture_0/sample_ready jam_gaussian_0/noise_out_TREADY

ad_ip_instance axi_gpio gaussian_capture_gpio
ad_ip_parameter gaussian_capture_gpio CONFIG.C_IS_DUAL 1
ad_ip_parameter gaussian_capture_gpio CONFIG.C_GPIO_WIDTH 32
ad_ip_parameter gaussian_capture_gpio CONFIG.C_GPIO2_WIDTH 32
ad_ip_parameter gaussian_capture_gpio CONFIG.C_ALL_OUTPUTS 1
ad_ip_parameter gaussian_capture_gpio CONFIG.C_ALL_INPUTS_2 1
ad_connect gaussian_capture_gpio/gpio_io_o gaussian_capture_0/control
ad_connect gaussian_capture_0/status gaussian_capture_gpio/gpio2_io_i
ad_cpu_interconnect 0x47050000 gaussian_capture_gpio

ad_ip_instance axi_bram_ctrl gaussian_capture_axi_bram
ad_ip_parameter gaussian_capture_axi_bram CONFIG.SINGLE_PORT_BRAM 1
ad_ip_instance blk_mem_gen gaussian_capture_bram
ad_ip_parameter gaussian_capture_bram CONFIG.Memory_Type {True_Dual_Port_RAM}
ad_ip_parameter gaussian_capture_bram CONFIG.Enable_32bit_Address false
ad_ip_parameter gaussian_capture_bram CONFIG.Write_Width_A 32
ad_ip_parameter gaussian_capture_bram CONFIG.Read_Width_A 32
ad_ip_parameter gaussian_capture_bram CONFIG.Write_Width_B 32
ad_ip_parameter gaussian_capture_bram CONFIG.Read_Width_B 32
ad_ip_parameter gaussian_capture_bram CONFIG.Write_Depth_A 1024
ad_connect gaussian_capture_axi_bram/BRAM_PORTA gaussian_capture_bram/BRAM_PORTA
ad_connect $sys_cpu_clk gaussian_capture_bram/clkb
ad_connect gaussian_capture_0/capture_write_en gaussian_capture_bram/enb
ad_connect gaussian_capture_0/capture_write_en gaussian_capture_bram/web
ad_connect gaussian_capture_0/capture_write_addr gaussian_capture_bram/addrb
ad_connect jam_gaussian_0/noise_out_TDATA gaussian_capture_bram/dinb
ad_cpu_interconnect 0x47040000 gaussian_capture_axi_bram
