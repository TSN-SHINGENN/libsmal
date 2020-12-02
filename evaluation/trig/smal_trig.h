#ifndef INC_SMAL_TRIG_H
#define INC_SMAL_TRIG_H

#if defined (__cplusplus )
extern "C" {
#endif

typedef struct _smal_trig {
    int i, k;
    long double x;
    long double t, tt, x2;
} smal_trig_t;

long double smal_abslf(const long double j);
long double smal_tanlf(smal_trig_t *const s, const long double x);
long double smal_sinlf(smal_trig_t *const s, const long double x);
long double smal_coslf(smal_trig_t *const s, const long double x);
long double smal_cos1lf(smal_trig_t *const s, const long double x);

#if defined (__cplusplus )
}
#endif

#endif /* INC_SMAL_TRIG_H */

