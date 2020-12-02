/**
 *	Copyright 2020 TSN-SHINGENN All Rights Reserved.
 *	Basic Author: Seiichi Takeda  '2020-November-18 Active
 */
 
#include <stddef.h>
#include <stdint.h>

#include "smal_math_def.h"
#include "smal_fix16.h"

#define own_round_u16(x) (uint16_t)((float)(x)+0.5)
#define own_round_u32(x) (uint32_t)((float)(x)+0.5)
#define own_round_i16(x) (int16_t)(((float)((x < 0) ? -x : x) + 0.5) * ((x < 0) ? -1.0 : 1.0));
#define own_round_i32(x) (int32_t)(((float)((x < 0) ? -x : x) + 0.5) * ((x < 0) ? -1.0 : 1.0));


/* •„†‚È‚µ */

void smal_fix16u_add(uint16_t *const y16_p, const uint16_t x16)
{
	*y16_p += x16;
}

void smal_fix16u_add2( smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2)
{
	s->tu16 = x16_1 + x16_2;
}

void smal_fix16u_sub(uint16_t *const y16_p, const uint16_t x16)
{
	*y16_p -= x16;
}

void smal_fix16u_mul(uint16_t *const y16_p, const uint16_t x16)
{
	*y16_p *= x16;
}

void smal_fix16u_mulf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
{
	s->tu16 = own_round_u16(f * (1 << fsft));
	*y16_p *= s->tu16;
}

void smal_fix16u_qmulf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
{
	s->tu32 = own_round_u16(f * (1 << fsft));
	s->tu32 *= *y16_p;
	s->tu32 >>= fsft;

	*y16_p = (uint16_t)s->tu32;
}

void smal_fix16u_qmul(smal_fix16_t *const s, uint16_t *const y16_p, const uint16_t x16, const uint8_t fsft)
{
	s->tu32 = *y16_p;
	s->tu32 *= x16;

	*y16_p = (uint16_t)( s->tu32 >> fsft);
}

void smal_fix16u_qmul2(smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2, uint8_t fsft)
{
	s->tu32 = x16_1 * x16_2;
	s->tu16 = (uint16_t)(s->tu32 >> fsft);
}

void smal_fix16u_mul2_u32(smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2)
{
	s->tu32 = (uint32_t)x16_1 * x16_2;
}

void smal_fix16u_divn(uint16_t *const y16_p, const uint16_t x16)
{
	*y16_p /= x16;
}

void smal_fix16u_divf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
{
	s->tu16 = own_round_u16(f * (1 << fsft));

	*y16_p  /= s->tu16;
}

void smal_fix16u_fdiv(smal_fix16_t *const s, const float f, const uint8_t fsft, uint16_t x16)
{
	s->tu32 = own_round_u32(f * (1 << fsft));
	s->tu16 = s->tu32 / x16;
}	

void smal_fix16u_round(uint16_t *const y16_p, const float f)
{
    *y16_p = own_round_u16(f);
}

void smal_fix16u_to16(uint16_t *const u16_p, const uint16_t un, const uint8_t fsft)
{
    *u16_p = (un << fsft);
}

void smal_fix16u_toun(uint16_t *const un_p, const uint16_t u16, const uint8_t fsft)
{
    *un_p = (u16 >> fsft);
}

void smal_fix16u_tof(smal_fix16_t *const s, float *const f, const uint16_t u16, const uint8_t fsft)
{
    s->tu16 = (1 << fsft);

    *f = (float)u16;
    *f /=  s->tu16;
}

void smal_fix16u_sqrt(smal_fix16_t *const s, uint16_t *const retv_p, uint16_t const x16, const uint8_t fst)
{
    static uint32_t last, t;
    if(x16>0) {
	t = s->tu32 = (uint32_t)x16 * ((uint32_t) 1 << fst );
	do {
	    last  = s->tu32;
	    s->tu32 = ((t / s->tu32) + s->tu32);
	    s->tu32 >>= 1;
	} while (s->tu32 < last);
	*retv_p = (uint16_t)last;
    } else if(x16==0) {
	*retv_p = 0;
    } else {
	*retv_p = ~0;
    }
    return;
}

/* •„†•t‚« */
void smal_fix16i_add(int16_t *const y16_p, const int16_t x16)
{
    *y16_p += x16;
}

void smal_fix16i_add2( smal_fix16_t *const s, const int16_t x16_1, const int16_t x16_2)
{
    s->ti16 = x16_1 + x16_2;
}

void smal_fix16i_sub(int16_t *const y16_p, const int16_t x16)
{
    *y16_p -= x16;
}

void smal_fix16i_mul(int16_t *const y16_p, const int16_t x16)
{
    *y16_p *= x16;
}

void smal_fix16i_mulf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
{
    s->ti16 = own_round_i16(f * (1 << fsft));
    *y16_p *= s->ti16;
}

void smal_fix16i_mul2_i32(smal_fix16_t *const s, const int16_t x16_1, const int16_t x16_2)
{
    s->ti32 = (int32_t)x16_1 * x16_2;
}

void smal_fix16i_qmul(smal_fix16_t *const s, int16_t *const y16_p, const int16_t x16, const uint8_t fsft)
{
    s->ti32 = *y16_p;
    s->ti32 *= x16;

    *y16_p = (uint16_t)( s->ti32 >> fsft);
}

void smal_fix16i_qmulf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
{
    s->ti32 = own_round_i16(f * (1 << fsft));
    s->ti32 *= *y16_p;
    s->ti32 >>= fsft;

    *y16_p = (uint16_t)s->ti32;
}

void smal_fix16i_divin(int16_t *const y16_p, const int16_t in)
{
    *y16_p /= in;
}

void smal_fix16i_divf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
{
    s->ti16 = own_round_i16(f * (1 << fsft));

    *y16_p  /= s->ti16;
}

void smal_fix16i_fdiv(smal_fix16_t *const s, const float f, const uint8_t fsft, int16_t x16)
{
    s->ti32 = own_round_i32(f * (1 << fsft));
    s->ti16 = s->ti32 / x16;
}

void smal_fix16i_round(int16_t *const y16_p, const float f)
{
    *y16_p = own_round_i16(f);
}

void smal_fix16i_to16(int16_t *const i16_p, const int16_t in, const uint8_t fsft)
{
    *i16_p = (in << fsft);
}

void smal_fix16i_toin(int16_t *const in_p, const int16_t i16, const uint8_t fsft)
{
    *in_p = ( i16 >> fsft);
}

void smal_fix16i_tof(smal_fix16_t *const s, float *const f, const int16_t i16, const uint8_t fsft)
{
    s->ti16 = ((int16_t)1 << fsft);
    *f = (float)i16;
    *f /= s->ti16;
}

void smal_fix16i_fto(int16_t *const retv_p, const uint8_t fsft, const float f)
{
    *retv_p = own_round_i16(f * ((int16_t)1 << fsft));
}

void smal_fix16i_abs(int16_t *const retv_p, const int16_t i16)
{
	*retv_p = (i16 < 0) ? -i16 : i16;
}

void smal_fix16i_powx2_u32(smal_fix16_t *const s, const int16_t x16)
{
    s->ti32 = (int32_t)x16 * x16;
    s->tu32 = s->ti32;
}

void smal_fix16i_qpowx2(smal_fix16_t *const s, const int16_t x16, const uint8_t fsft)
{
    s->ti32 = (int32_t)x16 * x16;
    s->ti32 >>= fsft;
    s->ti16 = (int16_t)s->ti32;
}

