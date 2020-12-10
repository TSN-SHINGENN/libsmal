/**
 *      Copyright 2020 TSN°SHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-Nov-10 Active
 *              Last Alteration $Author: takeda $
 *	Dual License :
 *	non-commercial ... MIT Licence
 *	    commercial ... Requires permission from the author
 */

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>

#include "smal_math_def.h"
#include "smal_trig.h"

#define N  19
#define D  4.4544551033807686783083602485579e-6

long double smal_abslf( const long double j)
{
    return (j >= 0) ? j : -j;
}

static void ur_tan(smal_trig_t *const s)
{
    s->k = (int)((s->x / SMAL_M_PI_2) + ((s->x >= 0) ? 0.5 : -0.5));

    s->x  = +(s->x - (3217.0 / 2048) * s->k) + D * s->k;
    s->x2 = s->x * s->x;

    s->tt = 0;
    for (s->i = N; s->i >= 3; s->i -= 2) {
	s->tt = s->x2 / (s->i - s->tt);
    }

    s->t = s->x / (1 - s->tt);
    return;
}

long double smal_tanlf(smal_trig_t *const s, const long double x)
{
    s->x = x;
    ur_tan(s);
    if (s->k % 2 == 0) {
	return s->t;
    }
    if (s->t != 0) {
	return -1 / s->t;
    }
    /* overflow */
    return HUGE_VAL;
}

long double smal_sinlf(smal_trig_t *const s, const long double x)
{
    s->x =(x / 2);
    ur_tan(s);
    s->t = 2 * s->t / (1 + s->t * s->t);
    if (s->k % 2 == 0) {
	return  s->t;
    }
    /* else */
    return -(s->t);
}

long double smal_coslf(smal_trig_t *const s, const long double x)
{
    return smal_sinlf(s, SMAL_M_PI_2 - smal_abslf(x));
}

long double smal_cos1lf(smal_trig_t *const s, const long double x)  /* 1 - cos(x) */
{
    s->x = smal_abslf(x / 2);
    ur_tan(s);
    s->t *= s->t;
    if(s->k % 2 == 0) {
	return 2 * s->t / (1 + s->t);
    }
    /* else */
    return 2 / (1 + s->t);
}

