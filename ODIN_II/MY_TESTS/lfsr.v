

module lfsr(
input [31:0]seed,
input rst,
input clk,
output reg [31:0]lfsrOut
);

reg [31:0]lfsr_value;

always @ (posedge clk or negedge rst) begin
	if (rst == 1'b0) begin
		lfsr_value <= seed;
	end
	else begin
		lfsr_value <= (lfsr_value << 1) 
		| (
		(lfsr_value[7] ^ 
		(lfsr_value[16] ^ 
		(lfsr_value[19] ^ 
		(lfsr_value[25] ^ 
		lfsr_value[29])))) );
	end
end

assign lfsrOut = lfsr_value;

endmodule
