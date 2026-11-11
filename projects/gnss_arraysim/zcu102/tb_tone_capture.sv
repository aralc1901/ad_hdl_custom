`timescale 1ns/1ps

module tb_tone_capture;
  reg clk = 1'b0;
  always #5 clk = ~clk;

  reg resetn = 1'b0;
  reg [31:0] control = 32'd0;
  wire [31:0] status;
  reg [31:0] s_axis_tdata = 32'd0;
  reg s_axis_tvalid = 1'b0;
  wire s_axis_tready;
  wire bram_write;
  wire [31:0] bram_addr;

  reg [31:0] memory [0:1023];
  integer writes = 0;
  integer i;
  integer old_writes;

  tone_capture dut (
    .clk(clk), .resetn(resetn), .control(control), .status(status),
    .s_axis_tvalid(s_axis_tvalid),
    .s_axis_tready(s_axis_tready),
    .bram_write(bram_write), .bram_addr(bram_addr)
  );

  always @(posedge clk) begin
    if (bram_write) begin
      if (bram_addr > 4092 || bram_addr[1:0] != 2'b00)
        $fatal(1, "BRAM byte address outside capacity or unaligned");
      memory[bram_addr[11:2]] <= s_axis_tdata;
      writes <= writes + 1;
    end
  end

  task automatic check(input bit condition, input string message);
    if (!condition) $fatal(1, "%s: status=%08x writes=%0d", message, status, writes);
  endtask

  task automatic arm(input [10:0] length);
    @(negedge clk);
    control = {5'd0, length, 16'd0};
    @(negedge clk);
    control[0] = 1'b1;
    @(negedge clk);
    control[0] = 1'b0;
  endtask

  task automatic send_word(input [31:0] value);
    @(negedge clk);
    s_axis_tdata = value;
    s_axis_tvalid = 1'b1;
    do @(posedge clk); while (!s_axis_tready);
    @(negedge clk);
    s_axis_tvalid = 1'b0;
  endtask

  initial begin
    repeat (4) @(negedge clk);
    resetn = 1'b1;

    // A producer may assert TVALID before arming. No transfer is accepted.
    s_axis_tdata = 32'h11112222;
    s_axis_tvalid = 1'b1;
    repeat (3) @(posedge clk);
    check(!s_axis_tready && writes == 0, "unarmed backpressure");
    arm(11'd4);
    @(posedge clk);
    @(negedge clk);
    s_axis_tvalid = 1'b0;
    check(status[10:0] == 11'd1 && memory[0] == 32'h11112222,
          "first accepted sample");

    // Pause in the middle of a burst; the offered word must not be written.
    control[1] = 1'b1;
    s_axis_tdata = 32'h33334444;
    s_axis_tvalid = 1'b1;
    old_writes = writes;
    repeat (4) @(posedge clk);
    check(!s_axis_tready && writes == old_writes && status[10:0] == 11'd1,
          "paused backpressure");
    @(negedge clk);
    control[1] = 1'b0;
    @(posedge clk);
    @(negedge clk);
    s_axis_tvalid = 1'b0;
    send_word(32'h55556666);
    send_word(32'h77778888);
    check(status[11] && !status[12] && status[10:0] == 11'd4,
          "four accepted samples complete");
    check(memory[0] == 32'h11112222 && memory[1] == 32'h33334444 &&
          memory[2] == 32'h55556666 && memory[3] == 32'h77778888,
          "sample order");

    old_writes = writes;
    s_axis_tvalid = 1'b1;
    s_axis_tdata = 32'hdeaddead;
    repeat (3) @(posedge clk);
    check(!s_axis_tready && writes == old_writes, "full backpressure");
    @(negedge clk);
    s_axis_tvalid = 1'b0;

    // Full capacity, including the last address and an exact count of 1024.
    arm(11'd1024);
    for (i = 0; i < 1024; i = i + 1)
      send_word(32'h80000000 | i);
    check(status[11] && status[10:0] == 11'd1024 && !s_axis_tready,
          "1024-word completion");
    check(memory[0] == 32'h80000000 && memory[1023] == 32'h800003ff,
          "first and final BRAM address");

    // Zero and over-capacity requests are rejected without opening TREADY.
    arm(11'd0);
    check(status[13] && !status[12] && !status[11] && status[10:0] == 0,
          "zero length rejected");
    arm(11'd1025);
    check(status[13] && !s_axis_tready && status[10:0] == 0,
          "over-capacity rejected");

    // An arm edge during capture reports an error without losing the burst.
    arm(11'd2);
    send_word(32'h12345678);
    arm(11'd7);
    check(status[14] && status[12] && status[10:0] == 11'd1,
          "busy rearm reported without clearing count");
    send_word(32'h9abcdef0);
    check(status[11] && status[14] && status[10:0] == 11'd2,
          "busy rearm leaves original length intact");
    arm(11'd1);
    check(!status[14] && !status[13] && !status[11] && status[10:0] == 0,
          "valid rearm clears sticky state");
    send_word(32'hfedcba98);
    check(status[11] && status[10:0] == 11'd1 &&
          memory[0] == 32'hfedcba98, "rearm captures new data");

    $display("PASS tone_capture: handshake, order, 1024 words, pause, errors, rearm");
    $finish;
  end
endmodule
