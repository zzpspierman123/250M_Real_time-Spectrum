module HS_AD9481(
	input		      sysclk_250m,
	input          sys_rst,
	input          dco_p,
	input          dco_n,
	
	input  [7:0]   din_a,
	input  [7:0]   din_b,
	
	output         adc_clk,
	output [7:0]   dout,
	output         pdwn
);

reg [7:0] din_a_r;
reg [7:0] din_b_r;

assign adc_clk = sysclk_250m;
assign pdwn    = ~sys_rst;

always @(posedge dco_n) 
begin 
	din_a_r <= din_a;
end 

always @(posedge dco_p) 
begin 
	din_b_r <= din_b;
end 

/* FIFO 读入读出 */
wire rdempty_sig;	//FIFO out 
reg  rdreq_en;	//set FIFO 

always @(posedge sysclk_250m) 
begin 
	if (!sys_rst) 
	begin 
		rdreq_en <= 1'b0;
	end 
	else 
	begin 
		rdreq_en <= ~rdempty_sig;
	end 
end 

FIFO	fifo_16to8 (
	.aclr		( ~sys_rst           ),
	.wrclk 	( dco_n              ),
	.wrreq 	( 1'b1               ),
	.data 	( {din_a_r, din_b_r} ),
	
	.rdclk 	( sysclk_250m        ),
	.rdreq 	( rdreq_en          ),
	.q 		( dout               ),
	.rdempty ( rdempty_sig        )
);
endmodule 