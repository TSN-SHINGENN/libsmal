/**
 *      Copyright 2020 TSNｰSHINGENN All Rights Reserved.
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
 * @brief 二つの変数に対する逆正接(Arc Tangent)の関数
 * @param ret_p x,yから求められる逆正接値
 * @param y 垂直座標(正規化）
 * @param x 水平座標(正規化)
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
 * @brief 逆正接(arc tangent)をCORDICアルゴリズムで計算する関数
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

/**
 * @fn double smal_atan(const double x)
 * @brief cordicアルゴリズムで計算するatan()。引数、戻り値ともにdouble値
 **/
double smal_atan(const double x)
{
    return (double)smal_atanl_cordic(FIX_CORDIC_STEPS, x);
}

/**
 * @fn double smal_atan(const double x)
 * @brief cordicアルゴリズムで計算するatan()。引数、戻り値ともにfloat値
 **/
float smal_atanf(float x)
{
    return (float)smal_atanl_cordic(FIX_CORDIC_STEPS, x);
}

/**
 * @fn double smal_atan(const double x)
 * @brief cordicアルゴリズムで計算するatan()。引数、戻り値ともにlong double値
 **/
long double smal_atanl(const long double x)
{
    return smal_atanl_cordic(FIX_CORDIC_STEPS, x);
}
