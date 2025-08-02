module magnitudeShifter ( input wire [30:0]  operant,
                          input wire         negative,
                          input wire [4:0]   shiftAmount,
                          output wire [30:0] result );

  wire [4:0] s_realShiftAmount = (negative == 1'b1) ? 5'd0 : shiftAmount;
  
  wire [30:0] s_shiftStage1 = (s_realShiftAmount[0] == 1'b1) ? {1'b0, operant[30:1]} : operant;
  wire [30:0] s_shiftStage2 = (s_realShiftAmount[1] == 1'b1) ? {2'd0, s_shiftStage1[30:2]} : s_shiftStage1;
  wire [30:0] s_shiftStage3 = (s_realShiftAmount[2] == 1'b1) ? {4'd0, s_shiftStage2[30:4]} : s_shiftStage2;
  wire [30:0] s_shiftStage4 = (s_realShiftAmount[3] == 1'b1) ? {8'd0, s_shiftStage3[30:8]} : s_shiftStage3;
  assign result = (s_realShiftAmount[4] == 1'b1) ? {16'd0, s_shiftStage4[30:16]} : s_shiftStage4;
endmodule
