#include <stdint.h>

#include "smal_hypots.h"

void smal_hypots16(smal_hypots16_t *const s, uint16_t *const retv_p, const int16_t x, const int16_t y)
{
    s->a = (uint32_t)x;
    s->b = (uint32_t)y;
    if(x < 0) {
	s->a = (int32_t)(-x);
    } else {
	s->a = (int32_t)x;
    }
    if(y < 0) {
	s->b = (int32_t)(-y);
    } else {
	s->b = (int32_t)y;
    }
    s->c = s->b;
    if(s->a != 0) {
        s->c = s->a;
        if (s->b != 0) {
            if(s->a < s->b) {
                s->a = s->b; s->b = s->c;
            }
            s->b *= s->b;
            s->c = s->b;
            s->b /= s->a;
            s->b *= 13;
            s->b >>= 5;
            s->b += s->a;    /*       0  < error < +1.9e-2 */
            s->a *= s->a;
            s->c += s->a;
            s->a = s->c;     /* apply newton-raphson methode twice */
            s->a /= s->b;
            s->a += s->b;
            s->a >>= 1;   /* -1.7e-4 < error <  0       */
            s->b = s->c;
            s->c /= s->a;
            s->c += s->a;
            s->c >>= 1;   /* -1.3e-8 < error <  0       */
            s->a = s->c;
            s->a++;
            s->a *= s->c;
            if (s->b > s->a) /* adjust LSB */
                s->c++;
        }
    }
    *retv_p = s->c;
}

