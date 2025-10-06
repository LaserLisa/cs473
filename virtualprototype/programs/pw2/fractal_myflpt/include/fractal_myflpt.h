#ifndef FRACTAL_MYFLPT_H
#define FRACTAL_MYFLPT_H

#include <stdint.h>
#include <stdio.h>

#define MANTISSE 23
#define EXPONENT 8
#define EXCESS 250

#define _SIGN_MASK      0x80000000
#define _EXPONENT_MASK  0x7F800000
#define _MANTISSE_MASK  0x007FFFFF

typedef union FloatAs32 {
    volatile float f;
    volatile uint32_t i; // "Overlays" other fields in union
} FloatAs32;

typedef int32_t my_float; // sign|exponent|mantissa

//! Function to multiply two floats using own type def
static inline my_float my_float_mul(my_float a, my_float b) {
    uint32_t sign_a = a & _SIGN_MASK;
    uint32_t sign_b = b & _SIGN_MASK;
    uint32_t exp_a = a & _EXPONENT_MASK;
    uint32_t exp_b = b & _EXPONENT_MASK;
    uint32_t mant_a = (a & _MANTISSE_MASK) | (1 << MANTISSE);
    uint32_t mant_b = (b & _MANTISSE_MASK) | (1 << MANTISSE);

    // add exponents
    uint32_t res_exp = (uint32_t)((exp_a - (EXCESS << MANTISSE)) + (exp_b - (EXCESS << MANTISSE)) + (EXCESS << MANTISSE));
    if (res_exp > (0xFF << MANTISSE)) res_exp = (0xFF << MANTISSE);       

    uint64_t res_mat = (uint64_t)mant_a * (uint64_t)mant_b;
    // print msb of res_mat and lsb
    if (res_mat & ((uint64_t)1 << 47)) { // 2* 23 +1 bits
        res_mat >>= 1;
        res_exp = res_exp + (1 << MANTISSE);
    }
    // truncate to 23 bits
    uint32_t new_mat = ((uint32_t)(res_mat >> MANTISSE)) & _MANTISSE_MASK;


    //check if need to witch sign bit
    uint32_t sign_bit = (a & _SIGN_MASK) ^ (b & _SIGN_MASK);

    my_float result = new_mat | res_exp | sign_bit;
    return result;

}

int ilog2(unsigned x);

//! Function to add two floats using own type def
static inline my_float my_float_add(my_float a, my_float b) {
    // Step 1: Extract exponent and fraction bits
    uint32_t sign_a = a & _SIGN_MASK;
    uint32_t sign_b = b & _SIGN_MASK;
    int32_t exp_a = ((a & _EXPONENT_MASK) >> MANTISSE)-EXCESS;
    int32_t exp_b = ((b & _EXPONENT_MASK) >> MANTISSE)-EXCESS;
    uint32_t frac_a = a & _MANTISSE_MASK;
    uint32_t frac_b = b & _MANTISSE_MASK;
    
    // Handle zero cases
    if (a == 0) return b;
    if (b == 0) return a;
    
    // append 1 to mantissa
    uint32_t mant_a = frac_a | (1 << MANTISSE);
    uint32_t mant_b = frac_b | (1 << MANTISSE);
    
    // compare exponents and shift smaller mantissa
    int32_t res_exp;
    if (exp_a >= exp_b) {
        res_exp = exp_a;
        uint32_t shift = exp_a - exp_b;
        if (shift > 0) {
            mant_b >>= shift;
        }
    } else {
        res_exp = exp_b;
        uint32_t shift = exp_b - exp_a;
        mant_a >>= shift;
    }
    
    // add mantissas
    uint32_t res_mant;
    uint32_t res_sign;
    
    if (sign_a == sign_b) {
        // Same sign: add mantissas
        res_mant = mant_a + mant_b;
        res_sign = sign_a;
    } else {
        // Different signs: subtract smaller from larger
        if (mant_a >= mant_b) {
            res_mant = mant_a - mant_b;
            res_sign = sign_a;
        } else {
            res_mant = mant_b - mant_a;
            res_sign = sign_b;
        }
    }

    if (res_mant == 0) {
        return 0;
    }
    
    // normalize mantissa and adjust exponent
    while (res_mant > (1 << (MANTISSE+1))) {  // More than 24 bits
        res_mant >>= 1;
        res_exp++;
    }

    
    while ((res_mant & (1 << MANTISSE)) == 0) {  // Less than 24 bits
        res_mant <<= 1;
        res_exp--;
    }

    
    // truncate to 23 bits
    uint32_t final_mant = res_mant & _MANTISSE_MASK;  // Remove implicit bit

    
    my_float result = res_sign | ((res_exp + EXCESS) << MANTISSE) | final_mant;
    return result;
}

//! Function to negate a float using own type def
static inline my_float my_float_neg(my_float a) {
    my_float result = a ^ _SIGN_MASK;
    return result;
}

//! Function to substract two floats using own type def
static inline my_float my_float_sub(my_float a, my_float b) {
    my_float result = my_float_add(a, my_float_neg(b));
    return result;
}


//! Function to convert integer to my_float
static inline my_float int_to_my_float(int x) {

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

//! Function to convert float to my_float
static inline my_float float_to_my_float(float x) {
    FloatAs32 f;
    f.f = x;

    uint32_t sign = f.i & _SIGN_MASK;
    uint32_t exponent = (f.i & _EXPONENT_MASK);
    uint32_t mantissa = f.i & _MANTISSE_MASK;
    exponent = exponent + (123 << MANTISSE); // add 123 to exponent
    my_float result = sign | exponent | mantissa;
    return result;
}

//! Function to convert my_float to float
static inline float my_float_to_float(my_float x) {
    uint32_t sign = x & _SIGN_MASK;
    uint32_t exponent = (x & _EXPONENT_MASK);
    uint32_t mantissa = x & _MANTISSE_MASK;
    exponent = exponent - (123 << MANTISSE); // add 123 to exponent
    uint32_t result = sign | exponent | mantissa;
    FloatAs32 f;
    f.i = result;
    float result_float = f.f;
    return result_float;
}

//! Function compare two floats and return 1 if a < b
static inline int my_float_less(my_float a, my_float b) {
    int32_t exp_a = ((a & _EXPONENT_MASK) >> MANTISSE)-EXCESS;
    int32_t exp_b = ((b & _EXPONENT_MASK) >> MANTISSE)-EXCESS;
    uint32_t frac_a = a & _MANTISSE_MASK;
    uint32_t frac_b = b & _MANTISSE_MASK;
    if (exp_a < exp_b) return 1;
    if (exp_a > exp_b) return 0;
    return frac_a < frac_b;
}


//! Colour type (5-bit red, 6-bit green, 5-bit blue)
typedef uint16_t rgb565;

//! \brief Pointer to fractal point calculation function
typedef uint16_t (*calc_frac_point_p)(my_float cx, my_float cy, uint16_t n_max);

uint16_t calc_mandelbrot_point_soft(my_float cx, my_float cy, uint16_t n_max);

//! Pointer to function mapping iteration to colour value
typedef rgb565 (*iter_to_colour_p)(uint16_t iter, uint16_t n_max);

rgb565 iter_to_bw(uint16_t iter, uint16_t n_max);
rgb565 iter_to_grayscale(uint16_t iter, uint16_t n_max);
rgb565 iter_to_colour(uint16_t iter, uint16_t n_max);

void draw_fractal(rgb565 *fbuf, int width, int height,
                  calc_frac_point_p cfp_p, iter_to_colour_p i2c_p,
                  my_float cx_0, my_float cy_0, my_float delta, uint16_t n_max);

#endif // FRACTAL_MYFLPT_H
