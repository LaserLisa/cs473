#ifndef FRACTAL_FXPT_H
#define FRACTAL_FXPT_H

#include <stdint.h>

typedef int32_t q7_25_t;
#define Q7_25_SHIFT 25

//! \brief Function to multiply two fixed point floats of format q7_25
//! \param a First operand
//! \param b Second operand
//! \return Result of multiplication
static inline q7_25_t q7_25_mul(q7_25_t a, q7_25_t b) {
    return (q7_25_t)(((int64_t)a * b) >> Q7_25_SHIFT);
}

//! \brief Function to convert integer to q7_25_t
//! \param x Integer to convert
//! \return Result of conversion
static inline q7_25_t int_to_q7_25(int x) {
    return x << Q7_25_SHIFT;
}

//! \brief Function to convert float to q7_25_t
//! \param x Float to convert
//! \return Result of conversion
static inline q7_25_t float_to_q7_25(float x) {
    return (q7_25_t)(x * (1 << Q7_25_SHIFT));
}

//! Colour type (5-bit red, 6-bit green, 5-bit blue)
typedef uint16_t rgb565;

//! \brief Pointer to fractal point calculation function
typedef uint16_t (*calc_frac_point_p)(q7_25_t cx, q7_25_t cy, uint16_t n_max);

uint16_t calc_mandelbrot_point_soft(q7_25_t cx, q7_25_t cy, uint16_t n_max);

//! Pointer to function mapping iteration to colour value
typedef rgb565 (*iter_to_colour_p)(uint16_t iter, uint16_t n_max);

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  q7_25_t cx_0, q7_25_t cy_0, q7_25_t delta, uint16_t n_max);

#endif // FRACTAL_FXPT_H
