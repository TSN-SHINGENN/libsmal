
#include <stddef.h>
#include <stdint.h>

#include "smal_pow.h"

double smal_ipow(const double x, const int n)
{
    static unsigned int abs_n;
    static double r=1.0, v;

    /* initialize */
    abs_n = (n<0) ? -n : n;
    r = 1.0;
    v = x;

    while(abs_n != 0) {
	if(abs_n & 1) {
	    r *= v;
	}
	v *= v;
	abs_n >>= 1;
    }

    if(n<0) {
	return 1/r;
    }

    return r;
}


double smal_ipow10(const int n)
{
    return smal_ipow(10.0, n);
}

