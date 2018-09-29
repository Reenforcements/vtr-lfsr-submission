/*------------------------------------------------------------------------------
 * This code was generated by Spiral Multiplier Block Generator, www.spiral.net
 * Copyright (c) 2006, Carnegie Mellon University
 * All rights reserved.
 * The code is distributed under a BSD style license
 * (see http://www.opensource.org/licenses/bsd-license.php)
 *------------------------------------------------------------------------------ */
/* ./multBlockGen.pl 29930 -fractionalBits 0*/
module multiplier_block (
    i_data0,
    o_data0
);

  // Port mode declarations:
  input   [31:0] i_data0;
  output  [31:0]
    o_data0;

  //Multipliers:

  wire [31:0]
    w1,
    w4096,
    w4097,
    w16,
    w4081,
    w17,
    w1088,
    w2993,
    w11972,
    w14965,
    w29930;

  assign w1 = i_data0;
  assign w1088 = w17 << 6;
  assign w11972 = w2993 << 2;
  assign w14965 = w2993 + w11972;
  assign w16 = w1 << 4;
  assign w17 = w1 + w16;
  assign w2993 = w4081 - w1088;
  assign w29930 = w14965 << 1;
  assign w4081 = w4097 - w16;
  assign w4096 = w1 << 12;
  assign w4097 = w1 + w4096;

  assign o_data0 = w29930;

  //multiplier_block area estimate = 8013.29760563677;
endmodule //multiplier_block

module surround_with_regs(
	i_data0,
	o_data0,
	clk
);

	// Port mode declarations:
	input   [31:0] i_data0;
	output  [31:0] o_data0;
	reg  [31:0] o_data0;
	input clk;

	reg [31:0] i_data0_reg;
	wire [30:0] o_data0_from_mult;

	always @(posedge clk) begin
		i_data0_reg <= i_data0;
		o_data0 <= o_data0_from_mult;
	end

	multiplier_block mult_blk(
		.i_data0(i_data0_reg),
		.o_data0(o_data0_from_mult)
	);

endmodule
