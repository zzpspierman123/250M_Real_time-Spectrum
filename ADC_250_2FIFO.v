module ADC_250_2FIFO ( 
	input        sys_rst,   
	input        sysclk_50m,          
	
	input        ADC_DCO_P,     
	input        ADC_DCO_N,        
	input  [7:0] ADC_DIN_A,     
	input  [7:0] ADC_DIN_B,       
	
	output 		 sysclk_250m,
	output       ADC_CLK,      
	output [7:0] ADC_DOUT,   
	output       ADC_PDWN, 

	output [7:0] uart_out,
	output TX,
	output reg wr_en,
	output tx_busy
);

//串口输出FIFO
reg wrreq_en2 = 1'b0;
wire wrfull_sig2;
reg rdreq_en2 = 1'b0;
wire rdempty_sig2;
//数据发送
//reg wr_en = 1'b0;
//wire tx_busy;
//wire [7:0] uart_out;

pll	pll_m ( 
	.inclk0(sysclk_50m),
	.areset(~sys_rst),	//高电平复位
	.c0(sysclk_250m),
	.locked()
);

HS_AD9481 HS_AD9481_m(
	.sysclk_250m(sysclk_250m),
	.sys_rst(sys_rst),
	.dco_p(ADC_DCO_P),
	.dco_n(ADC_DCO_N),
	
	.din_a(ADC_DIN_A),
	.din_b(ADC_DIN_B),
	
	.adc_clk(ADC_CLK),
	.dout(ADC_DOUT),
	.pdwn(ADC_PDWN)
);
////串口输出FIFO
//reg wrreq_en2;
//wire wrfull_sig2;
//reg rdreq_en2;
//wire rdempty_sig2;
always @(posedge wrfull_sig2 or posedge rdempty_sig2	or negedge sys_rst ) 
begin 
	if(!sys_rst)
		rdreq_en2 <= 1'b0;
		
	if(wrfull_sig2)//存满
	begin
	 wrreq_en2 <= 1'b0;
	 rdreq_en2 <= 1'b1;
	end 
	
	else if(rdempty_sig2)//读空
	begin 
	 wrreq_en2 <= 1'b1;	
	 rdreq_en2 <= 1'b0;
	end 	
end 

//reg rdreq_en2_lock = 1'b0;
//always @(posedge sysclk_250m)
//begin
//	if (txclk_en) 
//	begin
//	rdreq_en2_lock <= rdreq_en2;
//	end 
//end


FIFO2 FIFO2_m( 
	.aclr		( ~sys_rst ),
	
	.wrclk 	( sysclk_250m ),
	.wrreq 	( wrreq_en2	),
	.data 	( ADC_DOUT ),
	.wrfull	( wrfull_sig2 ),
	
	.rdclk 	( tx_busy ),//| rdreq_en2_lock  
	.rdreq 	( rdreq_en2 ),
	.q 		( uart_out ),
	.rdempty ( rdempty_sig2 ) 
);

//波特率
Baud_Rate UART_Baud_Rate(
	.clk_50m(sysclk_50m),
	.txclk_en(txclk_en)
);
////数据发送
//reg wr_en;
//wire tx_busy;
//wire [7:0] uart_out;
always @ (posedge wrfull_sig2 )
begin 
	if(wrfull_sig2)//FIFO存满
		wr_en <= 1'b1;
	else 
		wr_en <= 1'b0;
end

UART_TX UART_TX_DATA(
	.din(uart_out),
	.wr_en(wr_en),
	.clk_50m(sysclk_50m),
	.clken(txclk_en),//BUAD
	.tx(TX),
	.tx_busy(tx_busy)
);

endmodule 
