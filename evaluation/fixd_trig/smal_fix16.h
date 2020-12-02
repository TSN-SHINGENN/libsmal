#ifndef INC_SMAL_FIX16_H
#define INC_SMAL_FIX16_H

#include <stdint.h>

typedef struct _smal_fix16 {
    uint32_t tu32;
    uint16_t tu16;
    int16_t ti16;
    int32_t ti32;
} smal_fix16_t;

#if defined (__cplusplus )
extern "C" {
#endif

/* ïÑçÜÇ»Çµ */
void smal_fix16u_add(uint16_t *const y16_p, const uint16_t x16);
void smal_fix16u_add2( smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2);
void smal_fix16u_sub(uint16_t *const y16_p, const uint16_t x16);

void smal_fix16u_mul(uint16_t *const y16_p, const uint16_t x16);
void smal_fix16u_qmul(smal_fix16_t *const s, uint16_t *const y16_p, const uint16_t x16, const uint8_t fsft);

void smal_fix16u_mulf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft);
void smal_fix16u_qmulf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft);
void smal_fix16u_mul2_u32(smal_fix16_t *const s, const uint16_t y16, const uint16_t x16);

void smal_fix16u_divn(uint16_t *const y16_p, const uint16_t n);
void smal_fix16u_divf(smal_fix16_t *const s, uint16_t *const y16_p, const float f, const uint8_t fsft);
void smal_fix16u_fdiv(smal_fix16_t *const s, const float f, const uint8_t fsft, uint16_t x16);

void smal_fix16u_round(uint16_t *const y16_p, const float f);

void smal_fix16u_to16(uint16_t *const u16_p, const uint16_t un, const uint8_t fsft);
void smal_fix16u_toun(uint16_t *const un_p, const uint16_t u16, const uint8_t fsft);
void smal_fix16u_tof(smal_fix16_t *const s, float *const f, const uint16_t u16, const uint8_t fsft);
void smal_fix16u_sqrt(smal_fix16_t *const s, uint16_t *retv_p, uint16_t const x16, const uint8_t fst);

/* ïÑçÜïtÇ´ */
void smal_fix16i_add(int16_t *const y16_p, const int16_t x16);
void smal_fix16i_add2( smal_fix16_t *const s, const int16_t x16_1, const int16_t x16_2);
void smal_fix16i_sub(int16_t *const y16_p, const int16_t x16);

void smal_fix16i_mul(int16_t *const y16_p, const int16_t x16);

void smal_fix16i_mulf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft);
void smal_fix16i_mul2(smal_fix16_t *const s, const int16_t y16, const int16_t x16);
void smal_fix16i_mul2_i32(smal_fix16_t *const s, const int16_t y16, const int16_t x16);

void smal_fix16i_qmul(smal_fix16_t *const s, int16_t *const y16_p, const int16_t x16, const uint8_t fsft);
void smal_fix16i_qmulf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft);
void smal_fix16i_qmul2(smal_fix16_t *const s, const uint16_t x16_1, const uint16_t x16_2, uint8_t fsft);

void smal_fix16i_divin(int16_t *const y16_p, const int16_t n);
void smal_fix16i_divf(smal_fix16_t *const s, int16_t *const y16_p, const float f, const uint8_t fsft);
void smal_fix16i_fdiv(smal_fix16_t *const s, const float f, const uint8_t fsft, int16_t x16);

void smal_fix16i_round(int16_t *const y16_p, const float f);

void smal_fix16i_to16(int16_t *const u16_p, const int16_t un, const uint8_t fsft);
void smal_fix16i_toin(int16_t *const un_p, const int16_t u16, const uint8_t fsft);
void smal_fix16i_tof(smal_fix16_t *const s, float *const f, const int16_t i16, const uint8_t fsft);

void smal_fix16i_abs(int16_t *const retv_p, const int16_t i16);
void smal_fix16i_powx2_u32(smal_fix16_t *const s, const int16_t x16);

#if defined (__cplusplus )
}
#endif

#endif /* INC_SMAL_FIX16_H */
