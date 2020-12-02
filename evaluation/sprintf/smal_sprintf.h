#ifndef INC_SMAL_SPRINTF_H
#define INC_SMAL_SPRINTF_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

// #define USE_FLOAT_FORMAT

#if defined (__cplusplus )
extern "C" {
#endif

char * smal_strchr(const char *const s, const int c);

void smal_sprintf_init( void(*_putch_cb)(const char));
void smal_vprintf(void (*__putc)(char),  const char *const fmt, const va_list va);

void smal_printf(const char *const fmt, ...);

#if defined (__cplusplus )
}
#endif

#endif /* end of INC_SMAL_SPRINTF_H */

