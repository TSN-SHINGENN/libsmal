#ifndef INC_SMAL_SLIST_H
#define INC_SMAL_SLIST_H

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "smal_mallocater.h"

typedef struct _smal_slist {
   smal_mallocater_t mem;
   void *ext;
} smal_slist_t;

#if defined (__cplusplus )
extern "C" {
#endif

int smal_slist_init( smal_slist_t *const self_p, const uint8_t sizof_element, void *heapbuf, const uint32_t bufsz);

int smal_slist_destroy( smal_slist_t *const self_p);
int smal_slist_push( smal_slist_t *const self_p, const void *const el_p, const uint8_t sizof_element);
int smal_slist_pop( smal_slist_t *const self_p);
int smal_slist_front( smal_slist_t *const self_p, void *const el_p, const uint8_t  sizof_element );
int smal_slist_get_element_at( smal_slist_t *const self_p, const unsigned int num, void *const el_p, const uint8_t sizof_element);
int smal_slist_clear( smal_slist_t *const self_p);
uint16_t smal_slist_get_pool_cnt(smal_slist_t *const self_p);

int smal_slist_is_empty( smal_slist_t *const self_p);

int smal_slist_back( smal_slist_t *const self_p, void *const el_p, const uint8_t  sizof_element );
int smal_slist_insert_at( smal_slist_t *const self_p, const unsigned int no, void *const el_p, const uint8_t sizof_element);
int smal_slist_erase_at( smal_slist_t *const self_p, const unsigned int no);

#if defined (__cplusplus )
}
#endif

#endif /* end of INC_SMAL_SLIST_H */
