module leadingZeros( input wire [30:0] value,
                     output wire [4:0] result );

  /* first stage */
  reg [1:0]  s_nibble7;
  wire [2:0] s_nibble0, s_nibble1, s_nibble2, s_nibble3, s_nibble4, s_nibble5, s_nibble6;
  
  leadingZeroSmall n0 ( .value(value[3:0]),
                        .result(s_nibble0) );
  leadingZeroSmall n1 ( .value(value[7:4]),
                        .result(s_nibble1) );
  leadingZeroSmall n2 ( .value(value[11:8]),
                        .result(s_nibble2) );
  leadingZeroSmall n3 ( .value(value[15:12]),
                        .result(s_nibble3) );
  leadingZeroSmall n4 ( .value(value[19:16]),
                        .result(s_nibble4) );
  leadingZeroSmall n5 ( .value(value[23:20]),
                        .result(s_nibble5) );
  leadingZeroSmall n6 ( .value(value[27:24]),
                        .result(s_nibble6) );
  always @*
    case (value[30:28])
      3'b000  : s_nibble7 <= 2'd3;
      3'b001  : s_nibble7 <= 2'd2;
      3'b010  : s_nibble7 <= 2'd1;
      3'b011  : s_nibble7 <= 2'd1;
      default : s_nibble7 <= 2'd0;
    endcase

  /* second stage */
  wire [3:0] s_byte0 = (s_nibble1[2] == 1'b1) ? {1'b0,s_nibble1} + {1'b0,s_nibble0} : {1'b0,s_nibble1};
  wire [3:0] s_byte1 = (s_nibble3[2] == 1'b1) ? {1'b0,s_nibble3} + {1'b0,s_nibble2} : {1'b0,s_nibble3};
  wire [3:0] s_byte2 = (s_nibble5[2] == 1'b1) ? {1'b0,s_nibble5} + {1'b0,s_nibble4} : {1'b0,s_nibble5};
  wire [3:0] s_byte3 = (s_nibble7 == 2'b11) ? {2'b0,s_nibble7} + {1'b0,s_nibble6} : {2'b0,s_nibble7} ;
  
  /* third stage */
  wire [4:0] s_short0 = (s_byte1[3] == 1'b1) ? {1'b0,s_byte1} + {1'b0,s_byte0} : {1'b0,s_byte1};
  wire [4:0] s_short1 = (s_byte3 == 4'd7) ? {1'b0,s_byte3} + {1'b0,s_byte2} : {1'b0,s_byte3};
  
  /* last stage */
  assign result = (s_short1 == 5'd15) ? s_short1+s_short0 : s_short1;
endmodule
