module leadingZeroSmall ( input wire [3:0] value,
                          output reg [2:0] result );

  always @*
    case (value)
      4'b0000 : result <= 3'd4;
      4'b0001 : result <= 3'd3;
      4'b0010 : result <= 3'd2;
      4'b0011 : result <= 3'd2;
      4'b0100 : result <= 3'd1;
      4'b0101 : result <= 3'd1;
      4'b0110 : result <= 3'd1;
      4'b0111 : result <= 3'd1;
      default : result <= 3'd0;
    endcase
endmodule
