/**
 *	Copyright 2020 TSN-SHINGENN All Rights Reserved.
 *	Basic Author: Seiichi Takeda  '2020-November-18 Active
 *
 *	Dual License :
 *	non-commercial ... MIT Licence
 *	    commercial ... Requires permission from the author
 */
 
#include <stddef.h>
#include <stdint.h>

#include "smal_math_def.h"
#include "smal_fix32.h"

#define own_round_u32(x) (uint32_t)((float)(x)+0.5)
#define own_round_u64(x) (uint64_t)((float)(x)+0.5)
#define own_round_i32(x) (int32_t)(((float)((x < 0) ? -x : x) + 0.5) * ((x < 0) ? -1.0 : 1.0));
#define own_round_i64(x) (int64_t)(((float)((x < 0) ? -x : x) + 0.5) * ((x < 0) ? -1.0 : 1.0));

/* •„†‚È‚µ */

void smal_fix32u_add(uint32_t *const y32_p, const uint32_t x32)
{
    *y32_p += x32;
}

void smal_fix32u_add2( smal_fix32_t *const s, const uint32_t x32_1, const uint32_t x32_2)
{
    s->tu32 = x32_1 + x32_2;
}

void smal_fix32u_sub(uint32_t *const y32_p, const uint32_t x32)
{
    *y32_p -= x32;
}

void smal_fix32u_mul(uint32_t *const y32_p, const uint32_t x32)
{
    *y32_p *= x32;
}

void smal_fix32u_mulf(smal_fix32_t *const s, uint32_t *const y32_p, const float f, const uint8_t fsft)
{
    s->tu32 = own_round_u32(f * (1 << fsft));
    *y32_p *= s->tu32;
}

void smal_fix32u_mul2_u64(smal_fix32_t *const s, const uint32_t x16_1, const uint32_t x16_2)
{
    s->tu64 = (uint64_t)x16_1 * x16_2;
}

void smal_fix32u_divn(uint32_t *const y32_p, const uint32_t x32)
{
    *y32_p /= x32;
}

void smal_fix32u_to32(uint32_t *const u32_p, const uint32_t un, const uint8_t fsft)
{
    *u32_p = (un << fsft);
}

void smal_fix32u_toun(uint32_t *const un_p, const uint32_t u32, const uint8_t fsft)
{
    *un_p = (u32 >> fsft);
}

void smal_fix32u_sqrt(smal_fix32_t *const s, uint32_t *retv_p, uint32_t const x32, const uint8_t fst)
{
    static uint64_t last, t;

    if(x32>0) {
	t = s->tu64 = (uint64_t)x32 * ( (uint64_t)1 << fst );
	do {
	    last  = s->tu64;
	    s->tu64 = ((t / s->tu64) + s->tu64);
	    s->tu64 >>= 1;
	} while (s->tu64 < last);
	*retv_p = (uint32_t)last;
    } else {
	*retv_p = ~0;
    }
    return;
}


/* •„†•t‚« */

void smal_fix32i_add(int32_t *const y32_p, const int32_t x32)
{
    *y32_p += x32;
}

void smal_fix32i_add2( smal_fix32_t *const s, const int32_t x32_1, const int32_t x32_2)
{
    s->ti32 = x32_1 + x32_2;
}

void smal_fix32i_sub(int32_t *const y32_p, const int32_t x32)
{
    *y32_p -= x32;
}

void smal_fix32i_mul(int32_t *const y32_p, const int32_t x32)
{
    *y32_p *= x32;
}

void smal_fix32i_divin(int32_t *const y32_p, const int32_t x32)
{
    *y32_p /= x32;
}

void smal_fix32i_toin(int32_t *const in_p, const int32_t i32, const uint8_t fsft)
{
    *in_p = (i32 >> fsft);
}

void smal_fix32i_abs(int32_t *const retv_p,const int32_t i32)
{
    *retv_p = (i32<0) ? -i32 : i32;
}

void smal_fix32i_tof(smal_fix32_t *const s, float *const f, const int32_t i32, const uint8_t fsft)
{
    s->ti32 = ((int32_t)1 << fsft);
    *f = (float)i32;
    *f /= s->ti32;
}

void smal_fix32i_fto(int32_t *const retv_p, const uint8_t fsft, const float f)
{
    *retv_p = own_round_i32(f * ((int32_t)1 << fsft));
}

