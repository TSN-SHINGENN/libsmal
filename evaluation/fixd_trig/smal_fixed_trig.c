/**
 *      Copyright 2020 TSN°SHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-Nov-10 Active
 *              Last Alteration $Author: takeda $
 */

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>

#include "smal_math_def.h"
#include "smal_fix32.h"
#include "smal_fixed_trig.h"

#define N  19
#define D  4.4544551033807686783083602485579e-6



#if 0
static void ur_tan(uint32_t *const retv_p, const uint32_t x, int32_t *k)  /* tan(x)‚ÌŒ³ */
{
    static int i;
    static int32_t t, x2:

    *k = (int)(x / (PI / 2) + (x >= 0 ? 0.5 : -0.5));
    x = +(x - (3217.0 / 2048) * *k) + D * *k;
    x2 = x * x;  t = 0;
    for (i = N; i >= 3; i -= 2) {
	t = x2 / (i - t);
    }

    return x / (1 - t);
}
#endif
