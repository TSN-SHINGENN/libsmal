#ifndef INC_SMAL_HYPOTS_H
#define INC_SMAL_HYPOTS_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _smal_hypots16
{
    int32_t a, b, c;
} smal_hypots16_t;

void smal_hypots16(smal_hypots16_t *const s, uint16_t *const retv_p, const int16_t x, const int16_t y);

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_SMAL_HYPOTS_H */

