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

ARCHITECTURE platform_independent OF ff_ram IS

   TYPE REGTYPE IS ARRAY( 15 DOWNTO 0 ) OF std_logic_vector( 7 DOWNTO 0 );
   
   SIGNAL mem : REGTYPE;
   
BEGIN
   data_out <= mem(to_integer(unsigned(read_addr)));
   
   makeWrite : PROCESS( clock )
   BEGIN
      IF (rising_edge(clock)) THEN
         IF (WriteEnable = '1') THEN
            mem(to_integer(unsigned(write_addr))) <= data_in;
         END IF;
      END IF;
   END PROCESS makeWrite;

END platform_independent; 
