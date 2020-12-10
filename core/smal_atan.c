/**
 *      Copyright 2020 TSN°SHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-Nov-11 Active
 *              Last Alteration $Author: takeda $
 *
 *	Dual License :
 *	non-commercial ... MIT Licence
 *	    commercial ... Requires permission from the author
 */

#include <stdint.h>
#include <stdio.h>

#include "smal_math_def.h"
#include "smal_atan.h"

/**
 * @fn float smal_atan2f(const float y, const float x)
 * @brief “ñ‚Â‚Ì•Ï”‚É‘Î‚·‚é‹t³Ú(Arc Tangent)‚ÌŠÖ”
 * @param ret_p x,y‚©‚ç‹‚ß‚ç‚ê‚é‹t³Ú’l
 * @param y ‚’¼À•W(³‹K‰»j
 * @param x …•½À•W(³‹K‰»)
 **/
float smal_atan2f(const float y, const float x)
{
    volatile static double a, dret;

    a = y / x;

    if( x == 0.0 ) {
	if( y > 0.0 ) {
	    dret = SMAL_M_PI_2;
	} else if( y < 0.0 ) {
	    dret = -SMAL_M_PI_2;
	} else {
	    /* x = 0, y = 0 */
	    dret = 0.0;
	}
	return dret;
    } else if( x < 0.0 ) {
	dret = smal_atanl(a);
	if( y >= 0.0 ) {
	    dret += SMAL_M_PI;
	} else {
	    dret += (-SMAL_M_PI);
	}
	return dret;
    }

    /* x > 0.0 */
    return smal_atan(a);
}

/**
 * @fn void smal_atanl_cordic(const uint8_t num_steps, const long double a)
 * @brief ‹t³Ú(arc tangent)‚ðCORDICƒAƒ‹ƒSƒŠƒYƒ€‚ÅŒvŽZ‚·‚éŠÖ”
 */
long double smal_atanl_cordic(const uint8_t num_steps, const long double a)
{
    volatile static int8_t sgn = 0;
    volatile static uint8_t n=0;
    volatile static long double ldval=0.0, x=0.0;
    volatile static long double ldret=0;

    if( a > 1.0 ) {
	sgn = 1;
	x = 1.0 / a;
    } else if ( a < -1.0 ) {
	sgn = -1;
	x = 1.0 / a;
    } else {
	sgn = 0.0;
	x = a;
    }

    atanl_loop : {
	volatile static long double x2;
	volatile static long double n2;
	x2 = x * x;
	for(n=num_steps; n>=1; --n) {
	    n2 = n * n;

	    ldval = (long double)(( n2 * x2 ) / ( (2.0 * n) + (1.0 + ldval)));
	}
    }

    if(sgn > 0) {
	ldret = (long double)(((long double)SMAL_M_PI / 2.0) - (x / ( 1.0 + ldval)));
    } else if(sgn < 0) {
	ldret = (long double)(((long double)-SMAL_M_PI / 2.0) - (x / ( 1.0 + ldval)));
    } else {
        ldret = (long double)(x / ( 1.0 + ldval));
    }

    return ldret;
}

#define FIX_CORDIC_STEPS 15

double smal_atan(const double x)
{
    return (double)smal_atanl_cordic(FIX_CORDIC_STEPS, x);
}

float smal_atanf(float x)
{
    return (float)smal_atanl_cordic(FIX_CORDIC_STEPS, x);
}

long double smal_atanl(const long double x)
{
    return smal_atanl_cordic(FIX_CORDIC_STEPS, x);
}
