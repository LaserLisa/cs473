#ifndef FRACTAL_FXPT_H
#define FRACTAL_FXPT_H

#include <stdint.h>

typedef int32_t q4_28_t;

//! Function to multiply two fixed point floats of format q4_28
static inline q4_28_t q4_28_mul(q4_28_t a, q4_28_t b) {
    return (q4_28_t)(((int64_t)a * b) >> 28);
}

//! Function to convert integer to q4_28_t
static inline q4_28_t int_to_q4_28(int x) {
    return x << 28;
}

//! Function to convert float to q4_28_t
static inline q4_28_t float_to_q4_28(float x) {
    return (q4_28_t)(x * (1 << 28));
}

//! Colour type (5-bit red, 6-bit green, 5-bit blue)
typedef uint16_t rgb565;

//! \brief Pointer to fractal point calculation function
typedef uint16_t (*calc_frac_point_p)(q4_28_t cx, q4_28_t cy, uint16_t n_max);

uint16_t calc_mandelbrot_point_soft(q4_28_t cx, q4_28_t cy, uint16_t n_max);

//! Pointer to function mapping iteration to colour value
typedef rgb565 (*iter_to_colour_p)(uint16_t iter, uint16_t n_max);

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  q4_28_t cx_0, q4_28_t cy_0, q4_28_t delta, uint16_t n_max);

#endif // FRACTAL_FXPT_H
