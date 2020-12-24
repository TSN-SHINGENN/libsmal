#ifndef INC_SMAL_MALLOCATER_H
#define INC_SMAL_MALLOCATER_H

#pragma once

#include <stddef.h>

typedef struct _smal_mallocater {
    void *buf;
    unsigned int bufsize;
    unsigned int remain;
    void *fin;
} smal_mallocater_t;

extern smal_mallocater_t _smal_mallocater_heap_obj;

#if defined (__cplusplus )
extern "C" {
#endif

void smal_mallocater_init_obj(smal_mallocater_t *const self_p, void *const buf, const unsigned int bufsize);
void *smal_mallocater_alloc_with_obj(smal_mallocater_t *const self_p,const uint8_t size);
void smal_mallocater_free_with_obj(smal_mallocater_t *const self_p, void * const ptr);

unsigned int smal_mallocater_phys_with_obj(smal_mallocater_t *const self_p);
unsigned int smal_mallocater_avphys_with_obj(smal_mallocater_t *const self_p);

// void _smal_mallocater_dump_region_list(smal_mallocater_t const *const self_p);

#if defined (__cplusplus )
}
#endif

#endif /* end of INC_SMAL_MALLOCATER_H */
