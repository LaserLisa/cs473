#include "fractal_myflpt.h"
#include "swap.h"
#include "vga.h"
#include "cache.h"
#include "perf.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Constants describing the output device
const int SCREEN_WIDTH = 512;   //!< screen width
const int SCREEN_HEIGHT = 512;  //!< screen height

// Constants describing the initial view port on the fractal function
const float FRAC_WIDTH = 3.0; //!< default fractal width (3.0 in Q4.28)
// const my_float CX_0 = 0xFD800000;      //!< default start x-coordinate (-2.0 in Q4.28)
// const my_float CY_0 = 0xFD400000;      //!< default start y-coordinate (-1.5 in Q4.28)

const uint16_t N_MAX = 64;    //!< maximum number of iterations

// Helper function to print my_float as hex
void print_my_float_hex(const char* label, my_float x) {
    printf("%s: 0x%08X\n", label, (unsigned int)x);
}

int main() {
   volatile unsigned int *vga = (unsigned int *) 0x50000020;
   volatile unsigned int reg, hi;
   rgb565 frameBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
   float delta = FRAC_WIDTH / SCREEN_WIDTH;
   my_float mdelta = float_to_my_float(delta);
   my_float CX_0 = float_to_my_float(-2.0);
   my_float CY_0 = float_to_my_float(-1.5);
   int i;
   vga_clear();
   printf("Starting drawing a fractal\n");
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

   // test mandelbrot
   // float cx_float = 0.009765625;
   // float cy_float = -1.5;
   // my_float cx = float_to_my_float(cx_float);
   // my_float cy = float_to_my_float(cy_float);
   // uint16_t n = calc_mandelbrot_point_soft(cx, cy, N_MAX);
   // printf("n = %d\n", n);
   perf_init();

   perf_set_mask(PERF_COUNTER_RUNTIME, PERF_EXECUTED_INSTRUCTIONS_MASK);
   perf_start();
   draw_fractal(frameBuffer,SCREEN_WIDTH,SCREEN_HEIGHT,&calc_mandelbrot_point_soft, &iter_to_colour,CX_0,CY_0,mdelta,N_MAX);
   perf_stop();
   perf_print_time(PERF_COUNTER_RUNTIME, "Execution time:");
   
   // Test my_float implementation

   // float a = 0;
   // float b = 2.5;

   // FloatAs32 fa_float;
   // fa_float.f = a;
   // FloatAs32 fb_float;
   // fb_float.f = b;

   // printf("float a: 0x%08X\n", fa_float.i);
   // printf("float b: 0x%08X\n", fb_float.i);

   // my_float ma = float_to_my_float(a);
   // my_float mb = float_to_my_float(b);

   // // // my_float ma = CX_0;
   // // // my_float mb = CY_0;

   // float fa = my_float_to_float(ma);
   // float fb = my_float_to_float(mb);

   // fa_float.f = fa;
   // fb_float.f = fb;
   
   
   // print_my_float_hex("my_float a", ma);
   // print_my_float_hex("my_float b", mb);


   // my_float msum = my_float_add(ma, mb);
   // my_float mprod = my_float_mul(ma, mb);

   // float fmul = my_float_to_float(mprod);

   // FloatAs32 fmul_float;
   // fmul_float.f = fmul;

   // printf("my_float a * b as float: 0x%08X\n", fmul_float.i);

   // float fsum = my_float_to_float(msum);
   // FloatAs32 fsum_float;
   // fsum_float.f = fsum;
   // printf("my_float a + b as float: 0x%08X\n", fsum_float.i);
   

   // int a_int = 2;
   // int b_int = -0;
   // my_float ma_int = int_to_my_float(a_int);
   // my_float mb_int = int_to_my_float(b_int);
   // float fb_int = my_float_to_float(mb_int);
   // FloatAs32 fb_int_float;
   // fb_int_float.f = fb_int;
   // printf("my_float b as float: 0x%08X\n", fb_int_float.i);
   // my_float msum_int = my_float_add(ma_int, mb_int);
   // my_float mprod_int = my_float_mul(ma_int, mb_int);

   // float fmul_int = my_float_to_float(mprod_int);
   // FloatAs32 fmul_int_float;
   // fmul_int_float.f = fmul_int;
   // printf("my_float a * b as float: 0x%08X\n", fmul_int_float.i);

   // printf("int a: 0x%08X\n", a_int);
   // printf("int b: 0x%08X\n", b_int);
   // printf("my_float a: 0x%08X\n", ma_int);
   // printf("my_float b: 0x%08X\n", mb_int);
   // printf("my_float a + b: 0x%08X\n", msum_int);
   // printf("my_float a * b: 0x%08X\n", mprod_int);


#ifdef __OR1300__
   dcache_flush();
#endif
   printf("Done\n");
}
