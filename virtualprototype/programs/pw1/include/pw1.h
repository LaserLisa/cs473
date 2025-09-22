#ifndef PW1_H
#define PW1_H

/*
* Converts a given unsigned int number to string for the given base.
*
* @note requires (1) bufsz > 1 (2) base > 1
* @note append NUL character at the end of the output.
* @note writes buf[0] = 0 in case of failure.
*
* @return int 0 in case of overflow or invalid argument, or number of
* written chrarcters in case of success. (excluding NUL)
*/
unsigned int utoa(
    /** number to convert */
    unsigned int number,

    /** output buffer */
    char *buf,

    /** size of the output buffer */
    unsigned int bufsz,

    /** base (also the length of digits) */
    unsigned int base,

    /** digits in the base */
    const char *digits
);

#endif