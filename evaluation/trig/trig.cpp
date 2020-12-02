// trig.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPS 0.000001

#include "smal_math_def.h"
#include "smal_trig.h"

smal_trig_t st;

int main(void)  /* テスト用 */
{
    int i, err;
    long double x, s, c, c1, t;

    printf("x(deg) %12s %17s %17s %17s\n",
        "sin(x)", "cos(x)", "1-cos(x)", "tan(x)");
    for (i = -720; i <= 720; i += 15) {
        err = 0;
        x = i * (SMAL_M_PI / 180);
        s = smal_sinlf(&st, x);  c = smal_coslf(&st, x);  c1 = smal_cos1lf(&st, x);
        if (fabs((double)s - sin(x)) > EPS
            || fabs((double)c - cos(x)) > EPS
            || fabs((double)c + (double)c1 - 1) > EPS) err = 1;
        printf("%4d %17.14Lf %17.14Lf %17.14Lf", i, s, c, c1);
        if ((i + 90) % 180 != 0) {
            t = smal_tanlf(&st, x);
            if (fabs((double)t - tan(x)) > EPS) err = 1;
            printf(" %17.14Lf", t);
        }
        printf("\n");
        if (err) printf("|error| > %g\n", EPS);
    }
    return 0;
}
