module multiplyMyFlpt ( input wire [31:0]  valueA,
                                           valueB,
                        output wire [31:0] result );

  localparam signed [9:0] EXCESS_VALUE = 10'd250;

  wire        signA = valueA[31];
  wire        signB = valueB[31];
  wire signed [9:0]  expA  = {2'd0,valueA[7:0]};
  wire signed [9:0]  expB  = {2'd0,valueB[7:0]};
  wire [22:0] magA  = valueA[30:8];
  wire [22:0] magB  = valueB[30:8];
  
  wire signed [9:0] expAPlusExpB = expA + expB - EXCESS_VALUE;
  wire s_multSign = signA ^ signB;
  wire [45:0] multResult = magA*magB;
  wire [22:0] magResult = (multResult[45] == 1'b1) ? multResult[45:23] : multResult[44:22];
  wire signed [9:0] expAPlusExpBCorrected = (multResult[45] == 1'b1) ? expAPlusExpB : expAPlusExpB-10'd1;

  assign result = (magA == 23'd0  || magB == 23'd0) ? 32'd250 : 
                  (expAPlusExpBCorrected[9] == 1'b1) ? {s_multSign,31'h40000000} :
                  (expAPlusExpBCorrected[8] == 1'b1) ? {s_multSign,31'h7FFFFFFF} :
                  {s_multSign,magResult,expAPlusExpBCorrected[7:0]};
endmodule
