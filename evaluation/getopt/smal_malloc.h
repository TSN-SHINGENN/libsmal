#ifndef INC_SMAL_MALLOC_H
#define INC_SMAL_MALLOC_H

#pragma once

#include <stddef.h>
#include <stdint.h>

#pragma once

#if defined (__cplusplus )
extern "C" {
#endif

void smal_malloc_init(void *const buf, const uint8_t size);

void *smal_malloc(const uint8_t size);
void smal_free(void * const ptr);

#if defined (__cplusplus )
}
#endif

#endif /* end of INC_SMAL_MALLOC_H */
