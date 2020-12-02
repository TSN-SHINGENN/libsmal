#ifndef INC_SMAL_FIX32_H
#define INC_SMAL_FIX32_H

#include <stdint.h>

typedef struct _smal_fix32 {
    uint32_t tu32, ti32;
    uint64_t tu64, ti64;
} smal_fix32_t;

#if defined (__cplusplus )
extern "C" {
#endif

/* ïÑçÜÇ»Çµ */

void smal_fix32u_add(uint32_t *const y32_p, const uint32_t x32);
void smal_fix32u_add2(smal_fix32_t *const s, const uint32_t x32_1, const uint32_t x32_2);

void smal_fix32u_sub(uint32_t *const y32_p, const uint32_t x32);

void smal_fix32u_mul(uint32_t *const y32_p, const uint32_t x32);
void smal_fix32u_mul2_u64(smal_fix32_t *const s, const uint32_t y32, const uint32_t x32);


void smal_fix32u_divn(uint32_t *const y32_p, const uint32_t x32);

void smal_fix32u_to32(uint32_t *const u32_p, const uint32_t un, const uint8_t fsft);
void smal_fix32u_toun(uint32_t *const un_p, const uint32_t u32, const uint8_t fsft);

void smal_fix32u_sqrt(smal_fix32_t *const s, uint32_t *retv_p, uint32_t const x32, const uint8_t fst);

/* ïÑçÜïtÇ´ */
void smal_fix32i_add(int32_t *const y32_p, const int32_t x32);
void smal_fix32i_add2( smal_fix32_t *const s, const int32_t x32_1, const int32_t x32_2);
void smal_fix32i_sub(int32_t *const y32_p, const int32_t x32);
void smal_fix32i_mul(int32_t *const y32_p, const int32_t x32);
void smal_fix32i_divin(int32_t *const y32_p, const int32_t x32);
void smal_fix32i_toin(int32_t *const in_p, const int32_t i32, const uint8_t fsft);
void smal_fix32i_abs(int32_t *const retv_p,const int32_t i32);

void smal_fix32i_tof(smal_fix32_t *const s, float *const f, const int32_t i32, const uint8_t fsft);
void smal_fix32i_fto(int32_t *const retv_p, const uint8_t fsft, const float f);



#if defined (__cplusplus )
}
#endif

#endif /* INC_SMAL_FIX16_H */
