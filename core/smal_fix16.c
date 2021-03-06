/**
 *	Copyright 2020 TSN-SHINGENN All Rights Reserved.
 *	Basic Author: Seiichi Takeda  '2020-November-18 Active
 *	Dual License :
 *	non-commercial ... MIT Licence
 *	    commercial ... Requires permission from the author
 */

/**
 * @file smal_fix16.c
 * @brief 16bit 整数・固定小数点演算関数
 **/

#include <stddef.h>
#include <stdint.h>

#include "smal_math_def.h"
#include "smal_fix16.h"

#define own_round_u16(x) (uint16_t)((float)(x)+0.5)
#define own_round_u32(x) (uint32_t)((float)(x)+0.5)
#define own_round_i16(x) (int16_t)(((float)((x < 0) ? -x : x) + 0.5) * ((x < 0) ? -1.0 : 1.0));
#define own_round_i32(x) (int32_t)(((float)((x < 0) ? -x : x) + 0.5) * ((x < 0) ? -1.0 : 1.0));

/* 符号なし */

/**
 * @fn void smal_fix16u_add(uint16_t *const y16_p, const uint16_t x16)
 * @brief 加算 y+= x
 **/
void smal_fix16u_add(uint16_t *const y16_p, const uint16_t x16)
{
	*y16_p += x16;
}

/**
 * @fn void smal_fix16u_add2( smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2)
 * @brief 加算 s->tu16 = x16_1 + x16_2
 **/
void smal_fix16u_add2( smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2)
{
	s->tu16 = x16_1 + x16_2;
}

/**
 * @fn void smal_fix16u_sub(uint16_t *const y16_p, const uint16_t x16)
 * @brief 減算 y -= x
 **/
void smal_fix16u_sub(uint16_t *const y16_p, const uint16_t x16)
{
	*y16_p -= x16;
}

/**
 * @fn void smal_fix16u_mul(uint16_t *const y16_p, const uint16_t x16)
 * @brief 乗算 y *= x
 **/
void smal_fix16u_mul(uint16_t *const y16_p, const uint16_t x16)
{
	*y16_p *= x16;
}

/**
 * @fn void smal_fix16u_mulf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
 * @brief 浮動小数点を乗算 y *= f
 * @param fsft 小数点位置（LSBからのビット数)
 **/
void smal_fix16u_mulf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
{
	s->tu16 = own_round_u16(f * (1 << fsft));
	*y16_p *= s->tu16;
}

/**
 * @fn void smal_fix16u_qmulf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
 * @brief 固定小数点と浮動小数点の乗算 s->tu32 = y * f
 * @param fsft 小数点位置（LSBからのビット数)
 **/
void smal_fix16u_qmulf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
{
	s->tu32 = own_round_u16(f * (1 << fsft));
	s->tu32 *= *y16_p;
	s->tu32 >>= fsft;

	*y16_p = (uint16_t)s->tu32;
}

/**
 * @fn void smal_fix16u_qmul(smal_fix16_t *const s, uint16_t *const y16_p, const uint16_t x16, const uint8_t fsft)
 * @brief 固定小数点同士の乗算 y *= x;
 **/
void smal_fix16u_qmul(smal_fix16_t *const s, uint16_t *const y16_p, const uint16_t x16, const uint8_t fsft)
{
	s->tu32 = *y16_p;
	s->tu32 *= x16;

	*y16_p = (uint16_t)( s->tu32 >> fsft);
}

/**
 * @fn void smal_fix16u_qmul2(smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2, uint8_t fsft)
 * @brief 二つの固定小数点の乗算　s->tu16 = x16_1 * x16_2
 **/
void smal_fix16u_qmul2(smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2, uint8_t fsft)
{
	s->tu32 = x16_1 * x16_2;
	s->tu16 = (uint16_t)(s->tu32 >> fsft);
}

/**
 * @fn void smal_fix16u_mul2_u32(smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2)
 * @brief 二つの整数値の乗算 s->tu32 = x16_1 * x16_2
 **/
void smal_fix16u_mul2_u32(smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2)
{
	s->tu32 = (uint32_t)x16_1 * x16_2;
}

/**
 * @fn void smal_fix16u_divn(uint16_t *const y16_p, const uint16_t x16)
 * @brief 整数値での除算 y/=x
 **/
void smal_fix16u_divn(uint16_t *const y16_p, const uint16_t x16)
{
	*y16_p /= x16;
}

/**
 * @fn void smal_fix16u_divf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
 * @brief 浮動小数点値で固定小数点を除算　y /= f
 **/
void smal_fix16u_divf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft)
{
	s->tu16 = own_round_u16(f * (1 << fsft));

	*y16_p  /= s->tu16;
}

/**
 * @fn void smal_fix16u_fdiv(smal_fix16_t *const s, const float f, const uint8_t fsft, uint16_t x16)
 * @brief 浮動小数点値を整数値xで除算 s->tu16 = f / x16
 **/
void smal_fix16u_fdiv(smal_fix16_t *const s, const float f, const uint8_t fsft, uint16_t x16)
{
	s->tu32 = own_round_u32(f * (1 << fsft));
	s->tu16 = s->tu32 / x16;
}	

/**
 * @fn void smal_fix16u_round(uint16_t *const y16_p, const float f)
 * @brief 浮動小数点を四捨五入
 **/
void smal_fix16u_round(uint16_t *const y16_p, const float f)
{
    *y16_p = own_round_u16(f);
}

/**
 * @fn void smal_fix16u_to16(uint16_t *const u16_p, const uint16_t un, const uint8_t fsft)
 * @brief 整数値を固定小数点値として代入
 **/
void smal_fix16u_to16(uint16_t *const u16_p, const uint16_t un, const uint8_t fsft)
{
    *u16_p = (un << fsft);
}

/**
 * @fn void smal_fix16u_toun(uint16_t *const un_p, const uint16_t u16, const uint8_t fsft)
 * @brief 固定小数点値から整数値を代入
 **/
void smal_fix16u_toun(uint16_t *const un_p, const uint16_t u16, const uint8_t fsft)
{
    *un_p = (u16 >> fsft);
}

/**
 * @fn void smal_fix16u_tof(smal_fix16_t *const s, float *const f, const uint16_t u16, const uint8_t fsft)
 * @brief 固定小数点値から浮動小数点値へ変換
 **/
void smal_fix16u_tof(smal_fix16_t *const s, float *const f, const uint16_t u16, const uint8_t fsft)
{
    s->tu16 = (1 << fsft);

    *f = (float)u16;
    *f /=  s->tu16;
}

/**
 * @fn void smal_fix16u_sqrt(smal_fix16_t *const s, uint16_t *const retv_p, uint16_t const x16, const uint8_t fst)
 * @brief 固定小数点値の平方根を計算
 **/
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
    } else {
	*retv_p = ~0;
    }
    return;
}

/* 符号付き */

/**
 * @fn void smal_fix16i_add(int16_t *const y16_p, const int16_t x16)
 * @brief 加算 y += x
 **/ 
void smal_fix16i_add(int16_t *const y16_p, const int16_t x16)
{
    *y16_p += x16;
}

/**
 * @fn void smal_fix16i_add2( smal_fix16_t *const s, const int16_t x16_1, const int16_t x16_2)
 * @brief 加算 s->ti16 = x1 + x2
 **/
void smal_fix16i_add2( smal_fix16_t *const s, const int16_t x16_1, const int16_t x16_2)
{
    s->ti16 = x16_1 + x16_2;
}

/**
 * @fn void smal_fix16i_sub(int16_t *const y16_p, const int16_t x16)
 * @brief 減算 y -= x
 **/
void smal_fix16i_sub(int16_t *const y16_p, const int16_t x16)
{
    *y16_p -= x16;
}

/**
 * @fn void smal_fix16i_mul(int16_t *const y16_p, const int16_t x16)
 * @brief 乗算 y *= x
 **/
void smal_fix16i_mul(int16_t *const y16_p, const int16_t x16)
{
    *y16_p *= x16;
}

/**
 * @fn void smal_fix16i_mulf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
 * @brief 固定小数点と浮動小数点の乗算 y *= f
 **/
void smal_fix16i_mulf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
{
    s->ti16 = own_round_i16(f * (1 << fsft));
    *y16_p *= s->ti16;
}

/**
 * @fn void smal_fix16i_mul2_i32(smal_fix16_t *const s, const int16_t x16_1, const int16_t x16_2)
 * @brief 二つの整数値の乗算 s->ti32 = x1 * x2
 **/
void smal_fix16i_mul2_i32(smal_fix16_t *const s, const int16_t x16_1, const int16_t x16_2)
{
    s->ti32 = (int32_t)x16_1 * x16_2;
}

/**
 * @fn void smal_fix16i_qmul(smal_fix16_t *const s, int16_t *const y16_p, const int16_t x16, const uint8_t fsft)
 * @brief 固定小数点の乗算 y *= x
 **/
void smal_fix16i_qmul(smal_fix16_t *const s, int16_t *const y16_p, const int16_t x16, const uint8_t fsft)
{
    s->ti32 = *y16_p;
    s->ti32 *= x16;

    *y16_p = (uint16_t)( s->ti32 >> fsft);
}

/**
 * @fn void smal_fix16i_qmulf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
 * @brief 固定小数点値に浮動小数点を乗算
 **/
void smal_fix16i_qmulf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
{
    s->ti32 = own_round_i16(f * (1 << fsft));
    s->ti32 *= *y16_p;
    s->ti32 >>= fsft;

    *y16_p = (uint16_t)s->ti32;
}

/**
 * @fn void smal_fix16i_divin(int16_t *const y16_p, const int16_t in)
 * @brief 整数値の除算
 **/
void smal_fix16i_divin(int16_t *const y16_p, const int16_t in)
{
    *y16_p /= in;
}

/**
 * @fn void smal_fix16i_divf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
 * @brief 浮動小数点値の除算
 **/
void smal_fix16i_divf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft)
{
    s->ti16 = own_round_i16(f * (1 << fsft));

    *y16_p  /= s->ti16;
}

/**
 * @fn void smal_fix16i_fdiv(smal_fix16_t *const s, const float f, const uint8_t fsft, int16_t x16)
 * @brief 浮動小数点を固定小数点に変換して整数値で除算 s->ti16 = f / x
 */
void smal_fix16i_fdiv(smal_fix16_t *const s, const float f, const uint8_t fsft, int16_t x16)
{
    s->ti32 = own_round_i32(f * (1 << fsft));
    s->ti16 = s->ti32 / x16;
}

/**
 * @fn void smal_fix16i_round(int16_t *const y16_p, const float f)
 * @brief 浮動小数点値を四捨五入して整数値に
 */
void smal_fix16i_round(int16_t *const y16_p, const float f)
{
    *y16_p = own_round_i16(f);
}

/**
 * @fn void smal_fix16i_to16(int16_t *const i16_p, const int16_t in, const uint8_t fsft)
 * @brief 整数値を固定小数点に変換
 **/
void smal_fix16i_to16(int16_t *const i16_p, const int16_t in, const uint8_t fsft)
{
    *i16_p = (in << fsft);
}

/**
 * @fn void smal_fix16i_toin(int16_t *const in_p, const int16_t i16, const uint8_t fsft)
 * @brief 固定小数点値を整数に変換
 **/
void smal_fix16i_toin(int16_t *const in_p, const int16_t i16, const uint8_t fsft)
{
    *in_p = ( i16 >> fsft);
}

/**
 * @fn void smal_fix16i_tof(smal_fix16_t *const s, float *const f, const int16_t i16, const uint8_t fsft)
 * @nrief 固定小数点値を浮動小数点に変換
 **/
void smal_fix16i_tof(smal_fix16_t *const s, float *const f, const int16_t i16, const uint8_t fsft)
{
    s->ti16 = ((int16_t)1 << fsft);
    *f = (float)i16;
    *f /= s->ti16;
}

/**
 * @fn void smal_fix16i_fto(int16_t *const retv_p, const uint8_t fsft, const float f)
 * @brief 浮動小数点値を固定少数点値に変換
 **/
void smal_fix16i_fto(int16_t *const retv_p, const uint8_t fsft, const float f)
{
    *retv_p = own_round_i16(f * ((int16_t)1 << fsft));
}

/**
 * @fn void smal_fix16i_abs(int16_t *const retv_p, const int16_t i16)
 * @brief 絶対値を返す
 **/
void smal_fix16i_abs(int16_t *const retv_p, const int16_t i16)
{
	*retv_p = (i16 < 0) ? -i16 : i16;
}

/**
 * @fn void smal_fix16i_powx2_u32(smal_fix16_t *const s, const int16_t x16)
 * @brief 整数値 xの二乗を返す
 **/
void smal_fix16i_powx2_u32(smal_fix16_t *const s, const int16_t x16)
{
    s->ti32 = (int32_t)x16 * x16;
    s->tu32 = s->ti32;
}

/**
 * @fn void smal_fix16i_qpowx2(smal_fix16_t *const s, const int16_t x16, const uint8_t fsft)
 * @brief 固定小数点値 xの二乗を返す
 **/
void smal_fix16i_qpowx2(smal_fix16_t *const s, const int16_t x16, const uint8_t fsft)
{
    s->ti32 = (int32_t)x16 * x16;
    s->ti32 >>= fsft;
    s->ti16 = (int16_t)s->ti32;
}

