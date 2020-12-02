// fix16.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <stdio.h>

#include "smal_math_def.h"
#include "smal_fix16.h"
#include "smal_fix32.h"

#define ADC10_FULL 0x3ff
#define ADC8_FULL 255
#define ADC5_FULL 31
#define ADC10_LIMIT 768
#define ADC_GAIN (4.0 / 3.0)
#define ADC_GAIN_FIXQ10 1365
#define ADC_GAIN2_FIXQ10 2048
#define ADC_OFFSET (0x200) /* 9-bit */
 
#define ADC10_LOWER 50 /* 4*50 = 200 mV */

static uint16_t x16q10, y16q10, ulen16q10;
static int16_t ix16q10, iy16q10, ilen16q10;
static uint32_t ulen32q20, ulen32q10;
static int32_t ilen32q20, ilen32q10;
static uint16_t veclen10;

typedef struct {
    uint16_t x10, y10;
    uint16_t norrad10;
    uint16_t veclen10;
} adelement_t;


void atan_int_proc(adelement_t *const e)
{
    static smal_fix32_t tmp32;
    static smal_fix16_t tmp16;
    float f;

    /* 固定小数点Q10精度に変換 10ビット精度で 1.0とする */
    ix16q10 = e->x10;
    iy16q10 = e->y10;

    // smal_fix16u_tof(&tmp16, &f, ix16q10, 10);
    // fprintf( stdout, " x16q10 = %d(%f)\n", ix16q10, f);
    // smal_fix16u_tof(&tmp16, &f, iy16q10, 10);
    // fprintf( stdout, " y16q10 = %d(%f)\n", iy16q10, f);

    /* オフセット削除 */
    smal_fix16i_sub( &ix16q10, ADC_OFFSET);
    smal_fix16i_sub( &iy16q10, ADC_OFFSET);

    fprintf( stdout, " sub ADC_OFFSET(%d)\n", ADC_OFFSET);
    smal_fix16i_tof(&tmp16, &f, ix16q10, 10);
    fprintf( stdout, " x16q10 = %d(%f)\n", ix16q10, f);
    smal_fix16i_tof(&tmp16, &f, iy16q10, 10);
    fprintf( stdout, " y16q10 = %d(%f)\n", iy16q10, f);


    // fprintf( stdout, " mul ADC_GAIN\n");
    smal_fix16i_qmul(&tmp16, &ix16q10, ADC_GAIN2_FIXQ10, 10); /* 2.0 */
    smal_fix16i_qmul(&tmp16, &iy16q10, ADC_GAIN2_FIXQ10, 10); /* 2.0 */


    // smal_fix16i_tof(&tmp16, &f, ix16q10, 10);
    // fprintf( stdout, " x16q10 = %d(%f)\n", ix16q10, f);
    // smal_fix16i_tof(&tmp16, &f, iy16q10, 10);
    // fprintf( stdout, " y16q10 = %d(%f)\n", iy16q10, f);

    smal_fix16i_powx2_u32(&tmp16, ix16q10); /* Xの二乗 */
    ulen32q20 = tmp16.tu32;

    // xp = iy16q10;
    // fprintf( stdout, "x2 = %d\n", tmp16.tu32);
    smal_fix16i_powx2_u32(&tmp16, iy16q10); /* Yの二乗 */
    // yp = ulen32q20;
    ulen32q20 += tmp16.tu32;

#if 0 
    smal_fix32u_toun(&ulen32q10, ulen32q20, 10); /* 精度を落とす */
    ulen16q10 =  ulen32q10; /* キャストする */

    // xp = ulen16q10;
    smal_fix16i_tof(&tmp16, &f, ulen16q10, 10);
    fprintf( stdout, "ulen16q10 = %d(%f)\n", ulen16q10, f);

    smal_fix16u_sqrt(&tmp16, &ulen16q10, ulen16q10, 10);
#else
    smal_fix32u_sqrt(&tmp32, &ulen32q20, ulen32q20, 20);
    smal_fix32u_toun(&ulen32q10, ulen32q20, 10); /* 精度を落とす */
    ulen16q10 =  ulen32q10; /* キャストする */
#endif

    // yp = e->veclen10;
    smal_fix16i_tof(&tmp16, &f, ulen16q10, 10);
    fprintf( stdout, "sqrt() ulen16q10 = %d(%f)\n", ulen16q10, f);

    if( ulen16q10 < ADC10_FULL ) {
	e->veclen10 = ulen16q10;
    } else {
	e->veclen10 = ADC10_FULL;
    }
    // yp = e->veclen10;

    // fprintf( stdout, " e->veclen10 = %d(0x%x)\n", e->veclen10, e->veclen10);
    e->norrad10 = 0;

    return;
}

void atan_un_proc(void)
{
    static smal_fix32_t tmp32;
    static smal_fix16_t tmp16;
    float f;

    /* 固定小数点Q10精度に変換 10ビット精度で 1.0とする */
    x16q10 = 0x1ff;
    y16q10 = 0;

    smal_fix16i_tof(&tmp16, &f, x16q10, 10);
    fprintf( stdout, " x16q10 = %d(%f)\n", x16q10, f);
    smal_fix16i_tof(&tmp16, &f, y16q10, 10);
    fprintf( stdout, " y16q10 = %d(%f)\n", y16q10, f);

    fprintf( stdout, " mul ADC_GAIN\n");
#if 0
    smal_fix16u_qmulf(&tmp16, &x16q10, ADC_GAIN, 10);
    smal_fix16u_qmulf(&tmp16, &y16q10, ADC_GAIN, 10);
#else /* 最適化してくれないので、ゲインを先に計算した */
    smal_fix16u_qmul(&tmp16, &x16q10, ADC_GAIN_FIXQ10, 10); /* 4 / 3 */
    smal_fix16u_qmul(&tmp16, &y16q10, ADC_GAIN_FIXQ10, 10);
#endif
    smal_fix16u_tof(&tmp16, &f, x16q10, 10);
    fprintf( stdout, " x16q10 = %d(%f)\n", x16q10, f);
    smal_fix16u_tof(&tmp16, &f, y16q10, 10);
    fprintf( stdout, " y16q10 = %d(%f)\n", y16q10, f);

    smal_fix16u_mul2_u32(&tmp16, x16q10, x16q10); /* Xの二乗 */
    ulen32q20 = tmp16.tu32;
    fprintf( stdout, "x2 = %d\n", tmp16.tu32);
    smal_fix16u_mul2_u32(&tmp16, y16q10, y16q10); /* Yの二乗 */
    ulen32q20 += tmp16.tu32;
    fprintf( stdout, "y2 = %d\n", tmp16.tu32);
    fprintf( stdout, "ulen32q20 = %d\n", ulen32q20);
//    smal_fix32u_divn(&len32q20, 2);		 /* X2 + Y2 / 2 */
//    fprintf( stdout, "ulen32q20/2 = %d\n", ulen32q20);
    smal_fix32u_toun(&ulen32q10, ulen32q20, 10);	 /* 小数点精度を10ビットに落とす */
    ulen16q10 = (uint16_t)ulen32q10;		 /* キャストして整数精度切りつめ */

//    e->veclen10 = x16q10;
//    e->veclen10 = len16q10;
//    e->norrad10 = 0; /* atan計算しない */
//    e->norrad10 = x16q10;
    smal_fix16u_tof(&tmp16, &f, ulen16q10, 10);
    fprintf( stdout, " e->veclen10 = ulen16q10 = %d(%f)\n", ulen16q10, f);


    return;
}

static smal_fix16_t f16;

int main()
{
    uint16_t x = 10;
    uint16_t y = 3;
    uint32_t t32;
    size_t n;
    adelement_t a;

    fprintf( stdout, "y=%d, x=%d\n", y, x);

    smal_fix16u_add(&y, x); /* y += x */
    fprintf( stdout, "smal_fix16_add(&y, x)\n");
    fprintf( stdout, "y=%d, x=%d\n", y, x);

    smal_fix16u_sub(&y, x); /* y -= x */
    fprintf( stdout, "smal_sub16_sub(&y, x)\n");
    fprintf( stdout, "y=%d, x=%d\n", y, x);

    smal_fix16u_mul(&y, x); /* y *= x */
    fprintf( stdout, "smal_fix16_mul(&y, x)\n");
    fprintf( stdout, "y=%d, x=%d\n", y, x);

    smal_fix16u_mulf(&f16, &y, 2.5, 2); /* y *= 2.5(q2) */
    fprintf( stdout, "smal_fix16_mulf(&f16, &y, 2.5, 2)\n");
    fprintf( stdout, "y=%d, x=%d\n", y, x);

    smal_fix16u_divn(&y, 2); /* y /= 2 */
    fprintf( stdout, "smal_fix16_divn(&y, 2)\n");
    fprintf( stdout, "y=%d, x=%d\n", y, x);

    smal_fix16u_divf(&f16, &y, 0.6, 0); /* y /= 0.6(q0) */
    fprintf( stdout, "smal_fix16_divf(&f16, &y, 0.6, 0)\n");
    fprintf( stdout, "y=%d, x=%d\n", y, x);

    smal_fix16u_fdiv(&f16, 0.2345, 10, y);
    fprintf( stdout, "smal_fix16_fdiv(&f16, 0.2345, 10, y\n");
    fprintf( stdout, "f16.t16=%d\n", f16.tu16);

    smal_fix16u_mul2_u32(&f16, x, x);
    fprintf( stdout, "smal_fix16_mul2_u32(&f16, x, x)\n");
    fprintf( stdout, "f16.t32=%d, x=%d y=%d\n", f16.tu32, x, y);
    t32 = f16.tu32;
    smal_fix16u_mul2_u32(&f16, y, y);
    fprintf( stdout, "smal_fix16_mul2_u32(&f16, y, y)\n");
    fprintf( stdout, "f16.t32=%d, x=%d y=%d\n", f16.tu32, x, y);

    smal_fix32u_add(&t32, f16.tu32);
    fprintf( stdout, "smal_fix32_add(&t32, f16.t32)\n");
    fprintf( stdout, "t32=%d f16.t32=%d\n", t32, f16.tu32);

    smal_fix32u_divn(&t32, 2);
    fprintf( stdout, "smal_fix32_divn(&t32, 2)\n");
    fprintf( stdout, "t32=%d f16.t32=%d\n", t32, f16.tu32);

    x = t32;

    fprintf( stdout, "x=t32\n");
    fprintf( stdout, "x=%d t32=%d f16.t32=%d\n", x, t32, f16.tu32);
    for(n=0;n<10; ++n) {
	float f;
	smal_fix16u_tof(&f16, &f, x, (uint8_t)n); 
	fprintf( stdout, "x=%d smal_fix16_tof(&t16, x, %d)=%f\n", x, (int)n, f);
    }

    fprintf( stdout, "\natan_un_proc()\n");
    atan_un_proc();

    fprintf( stdout, "\natan_int_proc()\n");
#if 0
    for(n=0; n<ADC10_FULL+1; ++n) {
	a.x10 = n;
	a.y10 = n;
	atan_int_proc(&a);
	fprintf( stdout, "x10=0x%x y10=0x%x e->norrad10=0x%x e->veclen10=0x%x\n",
		a.x10, a.y10, a.norrad10, a.veclen10);
    }
#else
	a.x10 = ADC_OFFSET;
	a.y10 = ADC_OFFSET;
	atan_int_proc(&a);
	fprintf( stdout, "x10=0x%x y10=0x%x e->norrad10=0x%x e->veclen10=0x%x\n",
		a.x10, a.y10, a.norrad10, a.veclen10);
#endif

    return 0;
}




