#include <stdio.h>
#include <vga.h>
#include <spr.h>
#include "pw1.h"

const char *vigesimal_digits = "0123456789ABCDEFGHJ";
const unsigned int bufsz = 16;

int main () {
    char buf[bufsz]; // arrays are decays to pointers when passed to a function
    // char *buf = "000000000000000"; // works as well here but chatgpt says this could lead to undefined behaviour since strings are often stored in read-only memory
    unsigned int res;

    printf("Converting numbers 0 to 100 into vigesimal system:\n\n");

    for (unsigned int i = 1; i < 101; i++)
    {
        res = utoa(i, buf, bufsz, 20, vigesimal_digits);

        if (res == 0) {
            printf("FAILURE");
            break;
        }

        printf("%3d -> %s\n", i, buf);

    }
}

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
){
    unsigned int q = number;
    unsigned int r = 0;
    char temp[bufsz];
    unsigned int i = 0;

    if (base < 2 || bufsz < 1) {
        buf[0] = 0;
        return 0;
    }

    do {
        number = q;
        q = q/base;
        r = number % base; // remainder, digits in reverse
        temp[i++] = digits[r];
        
    } while ((q!=0 )&& (i<(bufsz-1)));

    if (i >= bufsz) {
        buf[0] = 0;
        return 0;
    }
    
    for (int j = 0; j < i; j++)
    {
        buf[j] = temp[i-1-j];
    }
    
    buf[i] = '\0';

    return i;
}
