// This module tests the division operator "/" (which really isn't division anymore.)

module main(
input clk,
input rst,
output reg [31:0]totalOutput
);

always @ (*) begin
	totalOutput = 32'd33434 / {clk, rst};
end

endmodule

