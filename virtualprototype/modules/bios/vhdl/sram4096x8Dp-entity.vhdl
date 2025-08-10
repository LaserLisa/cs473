library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sram4096x8Dp is
   port ( clockA       : in  std_logic;
          writeEnableA : in  std_logic;
          addressA     : in  unsigned( 11 downto 0 );
          dataInA      : in  std_logic_vector( 7 downto 0 );
          dataOutA     : out std_logic_vector( 7 downto 0 );
          clockB       : in  std_logic;
          writeEnableB : in  std_logic;
          addressB     : in  unsigned( 11 downto 0 );
          dataInB      : in  std_logic_vector( 7 downto 0 );
          dataOutB     : out std_logic_vector( 7 downto 0 ));
end sram4096x8Dp;
