// Finite, handshake-counted AXI-Stream capture for either GNSS generator.
// One accepted 32-bit I/Q sample is written to one BRAM word.
`timescale 1ns/1ps

module stream_capture (
  input  wire        clk,
  input  wire        resetn,
  input  wire [31:0] control,
  output wire [31:0] status,

  // Keep these scalar pins visible in IPI. The ADI interface catalogue has
  // generic VALID/EN/ADDR roles that otherwise create incomplete bus groups.
  (* X_INTERFACE_IGNORE = "true" *) input  wire        sample_valid,
  (* X_INTERFACE_IGNORE = "true" *) output wire        sample_ready,

  (* X_INTERFACE_IGNORE = "true" *) output wire        capture_write_en,
  (* X_INTERFACE_IGNORE = "true" *) output wire [31:0] capture_write_addr
);

  localparam [10:0] CAPACITY = 11'd1024;

  reg        arm_previous = 1'b0;
  reg        busy = 1'b0;
  reg        done = 1'b0;
  reg        invalid_length = 1'b0;
  reg        rearm_while_busy = 1'b0;
  reg [10:0] requested_length = 11'd0;
  reg [10:0] captured_count = 11'd0;

  wire arm_rise = control[0] && !arm_previous;
  wire [10:0] new_length = control[26:16];
  wire transfer = sample_valid && sample_ready;

  // A full or unarmed capture backpressures the producer. Pause allows the
  // same behavior to be exercised in the middle of a burst.
  assign sample_ready = busy && !control[1] && (captured_count < requested_length);

  // Port B of the true dual-port BRAM is a synchronous write port. On a
  // transfer edge the BRAM sees the old captured_count (the next free word).
  assign capture_write_en = transfer;
  // AXI BRAM Controller presents byte offsets to the BRAM generator. Match
  // its word-aligned addresses so PS offset 4*n reads captured sample n.
  assign capture_write_addr = {20'd0, captured_count[9:0], 2'b00};

  assign status = {16'd0, sample_ready, rearm_while_busy,
                   invalid_length, busy, done, captured_count};

  always @(posedge clk) begin
    if (!resetn) begin
      arm_previous     <= 1'b0;
      busy             <= 1'b0;
      done             <= 1'b0;
      invalid_length   <= 1'b0;
      rearm_while_busy <= 1'b0;
      requested_length <= 11'd0;
      captured_count   <= 11'd0;
    end else begin
      arm_previous <= control[0];

      if (arm_rise) begin
        if (busy) begin
          // Leave the current acquisition intact; report the bad rearm.
          rearm_while_busy <= 1'b1;
        end else begin
          captured_count   <= 11'd0;
          requested_length <= new_length;
          done             <= 1'b0;
          invalid_length   <= 1'b0;
          rearm_while_busy <= 1'b0;
          if ((new_length == 11'd0) || (new_length > CAPACITY)) begin
            invalid_length <= 1'b1;
            busy <= 1'b0;
          end else begin
            busy <= 1'b1;
          end
        end
      end

      if (transfer) begin
        captured_count <= captured_count + 11'd1;
        if (captured_count + 11'd1 == requested_length) begin
          busy <= 1'b0;
          done <= 1'b1;
        end
      end
    end
  end

endmodule
