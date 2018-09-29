// This module tests the division operator "/" (which really isn't division anymore.)

module main(
input clk,
input rst,
output reg [31:0]totalOutput1,
output reg [31:0]totalOutput2,
output reg [31:0]totalOutput3
);

	always @ (*) begin
		totalOutput1 = 32'd3 / {clk, rst};
	end

	always @ (*) begin
		totalOutput2 = 32'd389374879 / {clk, rst};
		totalOutput3 = totalOutput1 * (32'd983783 / {clk, rst});
	end

endmodule

