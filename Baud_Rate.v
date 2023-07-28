module Baud_Rate(
	input wire clk_50m,		
	output wire txclk_en
);

parameter BAUD = 115200;
parameter TX_ACC_MAX = 50000000 / BAUD ;
parameter TX_ACC_WIDTH = $clog2(TX_ACC_MAX);
reg [TX_ACC_WIDTH - 1:0] tx_acc = 1;

assign txclk_en = (tx_acc == 9'd1);

always @(posedge clk_50m) 
begin
	if(tx_acc == TX_ACC_MAX[TX_ACC_WIDTH - 1:0])
		tx_acc <= 0;
	else
		tx_acc <= tx_acc + 9'b1;
end

endmodule 