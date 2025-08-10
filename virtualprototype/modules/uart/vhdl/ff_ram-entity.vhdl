--------------------------------------------------------------------------------
-- $RCSfile: $
--
-- DESC    : OpenRisk 1420 
--
-- AUTHOR  : Dr. Theo Kluter
--
-- CVS     : $Revision: $
--           $Date: $
--           $Author: $
--           $Source: $
--
--------------------------------------------------------------------------------
--
--  HISTORY :
--
--  $Log: 
--------------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;

ENTITY ff_ram IS
   PORT ( clock      : IN  std_logic;
          data_in    : IN  std_logic_vector( 7 DOWNTO 0 );
          write_addr : IN  std_logic_vector( 3 DOWNTO 0 );
          WriteEnable: IN  std_logic;
          read_addr  : IN  std_logic_vector( 3 DOWNTO 0 );
          data_out   : OUT std_logic_vector( 7 DOWNTO 0 ));
END ff_ram;
