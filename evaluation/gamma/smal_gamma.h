#ifndef INC_GAMMA_H
#define INC_GAMMA_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

int smal_gamma8_init(uint8_t *const tab, const double g, const uint8_t offset, const uint16_t full_scale);

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_GAMMA_H */

