#ifndef FRACTAL_MYFLPT_H
#define FRACTAL_MYFLPT_H

#include <stdint.h>

#define MANTISSE 23

#define EXPONENT 8

#define EXCESS 250

typedef int32_t my_float;

//! Function to multiply two floats using own type def
static inline my_float my_float_mul(my_float a, my_float b) {
    
}

//! Function to add two floats using own type def
static inline my_float my_float_add(my_float a, my_float b) {
    
}

//! Function to substract two floats using own type def
static inline my_float my_float_sub(my_float a, my_float b) {
    
}

//! Function to convert integer to my_float
static inline my_float int_to_my_float(int x) {

}

//! Function to convert float to my_float
static inline my_float float_to_my_float(float x) {

}

//! Colour type (5-bit red, 6-bit green, 5-bit blue)
typedef uint16_t rgb565;

//! \brief Pointer to fractal point calculation function
typedef uint16_t (*calc_frac_point_p)(float cx, float cy, uint16_t n_max);

uint16_t calc_mandelbrot_point_soft(float cx, float cy, uint16_t n_max);

//! Pointer to function mapping iteration to colour value
typedef rgb565 (*iter_to_colour_p)(uint16_t iter, uint16_t n_max);

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  float cx_0, float cy_0, float delta, uint16_t n_max);

#endif // FRACTAL_MYFLPT_H
