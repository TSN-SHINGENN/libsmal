#ifndef INC_SMAL_ATAN_H
#define INC_SMAL_ATAN_H


#if defined(__cplusplus)
extern "C" {
#endif

float smal_atan2f(const float y, const float x);

long double smal_atanl_cordic(const uint8_t num_steps, const long double a);

double smal_atan(const double x);
float smal_atanf(float x);
long double smal_atanl(const long double x);

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_SMAL_ATAN_H */
