#ifndef INC_SMAL_MALLOCATER_H
#define INC_SMAL_MALLOCATER_H

#pragma once

#include <stddef.h>

typedef struct _smal_mallocater {
    void *buf;
    uint32_t bufsize;
    uint32_t remain;
    void *fin;
} smal_mallocater_t;

#if defined (__cplusplus )
extern "C" {
#endif

void smal_mallocater_init_obj(smal_mallocater_t *const self_p, void *const buf, const uint32_t bufsize);
void *smal_mallocater_alloc_with_obj(smal_mallocater_t *const self_p,const uint8_t size);
void smal_mallocater_free_with_obj(smal_mallocater_t*const self_p, void * const ptr);

// void _smal_mallocater_dump_region_list(smal_mallocater_t const *const self_p);

#if defined (__cplusplus )
}
#endif

#endif /* end of INC_SMAL_MALLOCATER_H */
