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

//! \brief Union to convert float to uint32_t to print in hex
typedef union FloatAs32 {
    volatile float f;
    volatile uint32_t i; 
} FloatAs32;

typedef uint32_t my_float; // sign|exponent|mantissa

//! \brief Function to calculate binary logarithm for unsigned integer argument x
//! \note  For x equal 0, the function returns -1.
//! \param x Argument
//! \return Result of calculation
int ilog2(unsigned x);

//! \brief Function to multiply two floats using own type def
//! \param a First operand
//! \param b Second operand
//! \return Result of multiplication
static inline my_float my_float_mul(my_float a, my_float b) {
    uint32_t sign_a = a & _SIGN_MASK;
    uint32_t sign_b = b & _SIGN_MASK;
    uint32_t exp_a = a & _EXPONENT_MASK;
    uint32_t exp_b = b & _EXPONENT_MASK;
    uint32_t mant_a = (a & _MANTISSE_MASK) | (1 << MANTISSE);
    uint32_t mant_b = (b & _MANTISSE_MASK) | (1 << MANTISSE);
    if (a == 0 || b == 0) return 0;

    // add exponents
    uint32_t res_exp = (uint32_t)(exp_a + exp_b - (EXCESS << MANTISSE));
    if (res_exp > (0xFF << MANTISSE)) res_exp = (0xFF << MANTISSE);       

    uint64_t res_mat = (uint64_t)mant_a * (uint64_t)mant_b;
    // print msb of res_mat and lsb
    if (res_mat & ((uint64_t)1 << 47)) { // 2* MANTISSE + 1 bits
        res_mat >>= 1;
        res_exp = res_exp + (1 << MANTISSE);
    }
    // truncate to 23 bits
    uint32_t new_mat = ((uint32_t)(res_mat >> MANTISSE)) & _MANTISSE_MASK;


    //check if need to witch sign bit
    uint32_t sign_bit = sign_a ^ sign_b;

    my_float result = new_mat | res_exp | sign_bit;
    return result;

}

//! \brief Function to add two floats using own type def
//! \param a First operand
//! \param b Second operand
//! \return Result of addition
static inline my_float my_float_add(my_float a, my_float b) {
    // Extract sign, exponent and fraction bits
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

    
    while ((res_mant & (1 << MANTISSE)) == 0 && res_mant != 0) {  // Less than 24 bits
        res_mant <<= 1;
        res_exp--;
    }

    // truncate to 23 bits
    uint32_t final_mant = res_mant & _MANTISSE_MASK;  // Remove implicit bit

    
    my_float result = res_sign | ((res_exp + EXCESS) << MANTISSE) | final_mant;
    return result;
}

//! \brief Function to negate a float using own type def
//! \param a Operand
//! \return Result of negation
static inline my_float my_float_neg(my_float a) {
    my_float result = a ^ _SIGN_MASK;
    return result;
}

//! \brief Function to substract two floats using own type def
//! \param a First operand
//! \param b Second operand
//! \return Result of subtraction
static inline my_float my_float_sub(my_float a, my_float b) {
    my_float result = my_float_add(a, my_float_neg(b));
    return result;
}


//! \brief Function to convert integer to my_float
//! \param x Integer to convert
//! \return Result of conversion
my_float int_to_my_float(int x);

//! \brief Function to convert float to my_float
//! \param x Float to convert
//! \return Result of conversion
my_float float_to_my_float(float x);

//! \brief Function to convert my_float to float
//! \param x my_float to convert
//! \return Result of conversion
float my_float_to_float(my_float x);


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
