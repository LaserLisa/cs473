module magnitudeCorrector( input wire [30:0] value,
                           input wire [4:0] shift,
                           output wire [22:0] result );

  wire [30:0] s_stage0 = (shift[0] == 1'b0) ? value : {value[29:0],1'b0};
  wire [30:0] s_stage1 = (shift[1] == 1'b0) ? s_stage0 : {s_stage0[28:0],2'd0};
  wire [30:0] s_stage2 = (shift[2] == 1'b0) ? s_stage1 : {s_stage1[26:0],4'd0};
  wire [30:0] s_stage3 = (shift[3] == 1'b0) ? s_stage2 : {s_stage2[22:0],8'd0};
  wire [30:0] s_stage4 = (shift[4] == 1'b0) ? s_stage3 : {s_stage3[14:0],16'd0};
  assign result = s_stage4[30:8];
endmodule
