module addMyFlpt ( input wire [31:0]  oppA,
                                      oppB,
                   output wire [31:0] result );

  wire        signA = oppA[31];
  wire        signB = oppB[31];
  wire signed [8:0]  expA  = {1'b0,oppA[7:0]};
  wire signed [8:0]  expB  = {1'b0,oppB[7:0]};
  wire [22:0] magA  = oppA[30:8];
  wire [22:0] magB  = oppB[30:8];

  wire signed [8:0] s_diffExpAB = expA - expB;
  wire signed [8:0] s_diffExpBA = expB - expA;
  wire              s_signsNotEqual = signA ^ signB;
  wire signed [9:0] s_selectedMag = (s_diffExpAB[8] == 1'b0) ? {1'd0, expA} : {1'd0, expB};

  /*
   *
   * Fast pass, one of the two values is out of the range of the other or both are zero
   *
   */
  wire s_aTooSmall   = (s_diffExpBA[8] == 1'b0 && s_diffExpBA[7:5] != 3'd0) ? 1'b1 : 1'b0;
  wire s_bTooSmall   = (s_diffExpAB[8] == 1'b0 && s_diffExpAB[7:5] != 3'd0) ? 1'b1 : 1'b0;
  wire s_bothAreZero = (magA == 23'd0 && magB == 23'd0) ? 1'b1 : 1'b0;
  
  /*
   *
   * adjust the magnitudes to the correct value
   *
   */
  wire [30:0] s_sum1, s_sum2;
  magnitudeShifter shiftA ( .operant({magA,8'd0}),
                            .negative(s_diffExpBA[8]),
                            .shiftAmount(s_diffExpBA[4:0]),
                            .result(s_sum1));
  magnitudeShifter shiftB ( .operant({magB,8'd0}),
                            .negative(s_diffExpAB[8]),
                            .shiftAmount(s_diffExpAB[4:0]),
                            .result(s_sum2));
  
  /*
   *
   * Do the adition in sign and magnitude
   *
   */
  wire s_magABiggerMagB = (s_sum1 >= s_sum2) ? 1'b1 : 1'b0;
  wire [31:0] s_magAPlusMagB = {1'b0,s_sum1} + {1'b0, s_sum2};
  wire [31:0] s_magAMinMagB = {1'b0,s_sum1} - {1'b0, s_sum2};
  wire [31:0] s_magBMinMagA = {1'b0, s_sum2} - {1'b0,s_sum1};
  wire        s_signResult = (s_signsNotEqual == 1'b0) ? signA : (s_magABiggerMagB == 1'b1) ? signA : signB;
  wire [31:0] s_magResult = (s_signsNotEqual == 1'b0) ? s_magAPlusMagB : (s_magABiggerMagB == 1'b1) ? s_magAMinMagB : s_magBMinMagA;
  
  /*
   *
   * finally we have to do the correction
   *
   */
  wire [4:0] s_nrOfLeadingZeros;
  wire [22:0] s_finalMag1;
  wire [22:0] s_finalMag = (s_magResult[31] == 1'b1) ? s_magResult[31:9] : s_finalMag1;
  wire signed [9:0] s_correctedMag = (s_magResult[31] == 1'b1) ? s_selectedMag + 10'd1 : s_selectedMag - {5'd0,s_nrOfLeadingZeros};
  
  leadingZeros lz ( .value(s_magResult[30:0]),
                    .result(s_nrOfLeadingZeros));
  
  magnitudeCorrector cor ( .value(s_magResult[30:0]),
                           .shift(s_nrOfLeadingZeros),
                           .result(s_finalMag1) );
  
  /*
   *
   * Here the result and done are defined
   *
   */
  assign result = (s_bothAreZero == 1'b1 || (s_nrOfLeadingZeros == 5'd31 && s_magResult[31] == 1'b0 && s_aTooSmall == 1'b0 && s_bTooSmall == 1'b0)) ? 32'd250 :
                  (s_aTooSmall == 1'b1) ? oppB :
                  (s_bTooSmall == 1'b1) ? oppA : 
                  (s_correctedMag[9] == 1'b1) ? {s_signResult,31'h40000000} :
                  (s_correctedMag[8] == 1'b1) ? {s_signResult,31'h7FFFFFFF} : {s_signResult,s_finalMag,s_correctedMag[7:0]};

endmodule
