#include "fractal_myflpt.h"
#include <swap.h>

//! \brief  Mandelbrot fractal point calculation function
//! \param  cx    x-coordinate
//! \param  cy    y-coordinate
//! \param  n_max maximum number of iterations
//! \return       number of performed iterations at coordinate (cx, cy)
uint16_t calc_mandelbrot_point_soft(my_float cx, my_float cy, uint16_t n_max) {
  my_float x = cx;
  my_float y = cy;
  uint16_t n = 0;
  my_float xx, yy, two_xy;
  my_float escape_cond = int_to_my_float(4);
  do {
    // FloatAs32 fx;
    // fx.f = my_float_to_float(x);
    // FloatAs32 fy;
    // fy.f = my_float_to_float(y);
    // printf("iteration %d: x = 0x%08X, y = 0x%08X\n", n, fx.i, fy.i);
    xx = my_float_mul(x, x);
    yy = my_float_mul(y, y);
    two_xy = my_float_mul(my_float_mul(x, y), int_to_my_float(2));

    x = my_float_add(my_float_sub(xx, yy), cx);
    y = my_float_add(two_xy, cy);
    ++n;
  } while (my_float_add(xx, yy) < escape_cond && (n < n_max));
  // printf("while loop ended\n");
  return n;
}

//! \brief Function to convert integer to my_float
//! \param x Integer to convert
//! \return Result of conversion
my_float int_to_my_float(int x) {

  if (x == 0) return 0;
  // check if x is out of range
  if (x > 63) x = 63;
  if (x < -63) x = -63;
  uint32_t sign;

  if (x < 0 ) {
      x = -x;
      sign = _SIGN_MASK;
  }
  else {
      sign = 0;
  }

  // Find the highest bit set. Add the bias to its index, that's gonna be your exponent.
  int highest_bit = ilog2((uint32_t)x);
  uint32_t exponent = (highest_bit + EXCESS) << MANTISSE;


  // Clear the highest bit set, what remains is the mantissa.
  uint32_t mantissa = (x ^ (1 << highest_bit)) << (MANTISSE-highest_bit);


  my_float result = sign | exponent | mantissa;

  return result;
}

//! \brief Function to convert float to my_float
//! \param x Float to convert
//! \return Result of conversion
my_float float_to_my_float(float x) {
  if (x == 0) return 0;
  FloatAs32 f;
  f.f = x;

  uint32_t sign = f.i & _SIGN_MASK;
  uint32_t exponent = (f.i & _EXPONENT_MASK);
  uint32_t mantissa = f.i & _MANTISSE_MASK;
  exponent = exponent + ((EXCESS-127) << MANTISSE); // add 123 (250-127) to exponent
  my_float result = sign | exponent | mantissa;
  return result;
}

//! \brief Function to convert my_float to float
//! \param x my_float to convert
//! \return Result of conversion
float my_float_to_float(my_float x) {
  if (x == 0) return 0;
  uint32_t sign = x & _SIGN_MASK;
  uint32_t exponent = (x & _EXPONENT_MASK);
  uint32_t mantissa = x & _MANTISSE_MASK;
  exponent = exponent - ((EXCESS-127) << MANTISSE); // add 123 (250-127) to exponent
  uint32_t result = sign | exponent | mantissa;
  FloatAs32 f;
  f.i = result;
  float result_float = f.f;
  return result_float;
}

//! \brief  Map number of performed iterations to black and white
//! \param  iter  performed number of iterations
//! \param  n_max maximum number of iterations
//! \return       colour
rgb565 iter_to_bw(uint16_t iter, uint16_t n_max) {
  if (iter == n_max) {
    return 0x0000;
  }
  return 0xffff;
}


//! \brief  Map number of performed iterations to grayscale
//! \param  iter  performed number of iterations
//! \param  n_max maximum number of iterations
//! \return       colour
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max) {
  if (iter == n_max) {
    return 0x0000;
  }
  uint16_t brightness = iter & 0xf;
  return swap_u16(((brightness << 12) | ((brightness << 7) | brightness<<1)));
}


//! \brief Calculate binary logarithm for unsigned integer argument x
//! \note  For x equal 0, the function returns -1.
int ilog2(unsigned x) {
  if (x == 0) return -1;
  int n = 1;
  if ((x >> 16) == 0) { n += 16; x <<= 16; }
  if ((x >> 24) == 0) { n += 8; x <<= 8; }
  if ((x >> 28) == 0) { n += 4; x <<= 4; }
  if ((x >> 30) == 0) { n += 2; x <<= 2; }
  n -= x >> 31;
  return 31 - n;
}


//! \brief  Map number of performed iterations to a colour
//! \param  iter  performed number of iterations
//! \param  n_max maximum number of iterations
//! \return colour in rgb565 format little Endian (big Endian for openrisc)
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max) {
  if (iter == n_max) {
    return 0x0000;
  }
  uint16_t brightness = (iter&1)<<4|0xF;
  uint16_t r = (iter & (1 << 3)) ? brightness : 0x0;
  uint16_t g = (iter & (1 << 2)) ? brightness : 0x0;
  uint16_t b = (iter & (1 << 1)) ? brightness : 0x0;
  return swap_u16(((r & 0x1f) << 11) | ((g & 0x1f) << 6) | ((b & 0x1f)));
}

rgb565 iter_to_colour1(uint16_t iter, uint16_t n_max) {
  if (iter == n_max) {
    return 0x0000;
  }
  uint16_t brightness = ((iter&0x78)>>2)^0x1F;
  uint16_t r = (iter & (1 << 2)) ? brightness : 0x0;
  uint16_t g = (iter & (1 << 1)) ? brightness : 0x0;
  uint16_t b = (iter & (1 << 0)) ? brightness : 0x0;
  return swap_u16(((r & 0xf) << 12) | ((g & 0xf) << 7) | ((b & 0xf)<<1));
}

//! \brief  Draw fractal into frame buffer
//! \param  width  width of frame buffer
//! \param  height height of frame buffer
//! \param  cfp_p  pointer to fractal function
//! \param  i2c_p  pointer to function mapping number of iterations to colour
//! \param  cx_0   start x-coordinate
//! \param  cy_0   start y-coordinate
//! \param  delta  increment for x- and y-coordinate
//! \param  n_max  maximum number of iterations
void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  my_float cx_0, my_float cy_0, my_float delta, uint16_t n_max) {
  rgb565 *pixel = fbuf;
  my_float cy = cy_0;
  FloatAs32 fcy;
  FloatAs32 fcx;

  fcx.f = my_float_to_float(cx_0);
  fcy.f = my_float_to_float(cy_0);
  for (int k = 0; k < height; ++k) {
    my_float cx = cx_0;
    for(int i = 0; i < width; ++i) {
      uint16_t n_iter = (*cfp_p)(cx, cy, n_max);
      rgb565 colour = (*i2c_p)(n_iter, n_max);
      *(pixel++) = colour;
      cx = my_float_add(cx, delta);
      fcx.f = my_float_to_float(cx);
      // printf("X iteration %d: x : 0x%08X  y : 0x%08X\n ", i, fcx.i, fcy.i);
    }
    cy = my_float_add(cy, delta);
    fcy.f = my_float_to_float(cy);
    // printf("iteration %d: y : 0x%08X\n ", k, fcy.i);
  }
}
