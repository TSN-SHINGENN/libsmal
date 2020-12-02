#ifndef INC_SMAL_FIXED_TRIG_H
#define INC_SMAL_FIXED_TRIG_H

#if defined (__cplusplus )
extern "C" {
#endif

void smal_fix32q28_tan(uint32_t *const r, const uint32_t x);
void smal_fix32q28_sin(uint32_t *const r, const uint32_t x);
void smal_fix32q28_cos(uint32_t *const r, const uint32_t x);
void smal_fix32q28_cos1(uint32_t *const r, const uint32_t x);

#if defined (__cplusplus )
}
#endif

#endif /* INC_SMAL_FIXED_TRIG_H */

