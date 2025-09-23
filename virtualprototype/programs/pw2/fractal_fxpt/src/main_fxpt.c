#include "fractal_fxpt.h"
#include "swap.h"
#include "vga.h"
#include "cache.h"
#include <stddef.h>
#include <stdio.h>

// Constants describing the output device
const int SCREEN_WIDTH = 512;   //!< screen width
const int SCREEN_HEIGHT = 512;  //!< screen height

// Constants describing the initial view port on the fractal function
const float FRAC_WIDTH = 3.0; //!< default fractal width (3.0 in Q4.28)
const q4_28_t CX_0 = 0xE0000000;      //!< default start x-coordinate (-2.0 in Q4.28)
const q4_28_t CY_0 = 0xE8000000;      //!< default start y-coordinate (-1.5 in Q4.28)
const uint16_t N_MAX = 64;    //!< maximum number of iterations

int main() {
   // everything here seems to work
   // printf("FRAC_WIDTH = 0x%08X\n", FRAC_WIDTH);
   // printf("CX_0 = 0x%08X\n", CX_0);
   // printf("CY_0 = 0x%08X\n", CY_0);
   // printf("int to q428 (5): 0x%08X\n", float_to_q4_28(-2.0));
   // printf("multiplication (1.6879*2.6502) 0x%08X\n", q4_28_mul(float_to_q4_28(1.6879), float_to_q4_28(2.6502)));
   
   volatile unsigned int *vga = (unsigned int *) 0x50000020;
   volatile unsigned int reg, hi;
   rgb565 frameBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
   q4_28_t delta = float_to_q4_28((FRAC_WIDTH / SCREEN_WIDTH));
   int i;
   vga_clear();
   printf("Starting drawing a fractal\n");
   // uint16_t n = calc_mandelbrot_point_soft(float_to_q4_28(0.2), float_to_q4_28(0.7), N_MAX);
   // printf("Number of iterations %d\n", n);
#ifdef __OR1300__   
   /* enable the caches */
   icache_write_cfg( CACHE_DIRECT_MAPPED | CACHE_SIZE_8K | CACHE_REPLACE_FIFO );
   dcache_write_cfg( CACHE_FOUR_WAY | CACHE_SIZE_8K | CACHE_REPLACE_LRU | CACHE_WRITE_BACK );
   icache_enable(1);
   dcache_enable(1);
#endif
   /* Enable the vga-controller's graphic mode */
   vga[0] = swap_u32(SCREEN_WIDTH);
   vga[1] = swap_u32(SCREEN_HEIGHT);
   vga[2] = swap_u32(1);
   vga[3] = swap_u32((unsigned int)&frameBuffer[0]);
   /* Clear screen */
   for (i = 0 ; i < SCREEN_WIDTH*SCREEN_HEIGHT ; i++) frameBuffer[i]=0;

   draw_fractal(frameBuffer,SCREEN_WIDTH,SCREEN_HEIGHT,&calc_mandelbrot_point_soft, &iter_to_colour,CX_0,CY_0,delta,N_MAX);
#ifdef __OR1300__
   dcache_flush();
#endif
   printf("Done\n");
}
