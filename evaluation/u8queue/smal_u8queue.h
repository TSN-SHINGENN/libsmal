#ifndef INC_SMAL_U8QUEUE_H
#define INC_SMAL_U8QUEUE_H

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct _smal_u8queue_ {
    size_t queue_size;
    unsigned int head; /* �L���[�̐擪�ʒu */
    unsigned int tail; /* �L���[�̖����ʒu */
    uint8_t *buf;
} smal_u8queue_t;

#if defined (__cplusplus )
extern "C" {
#endif

int smal_u8queue_init(smal_u8queue_t *const self_p, const uint8_t size);
int smal_u8queue_is_empty(smal_u8queue_t *const self_p);
int smal_u8queue_push(smal_u8queue_t *const self_p, const uint8_t u8);
int smal_u8queue_pop(smal_u8queue_t *const self_p, uint8_t *const u8_p);
int smal_u8queue_destroy(smal_u8queue_t *const self_p);

#if defined (__cplusplus )
}
#endif

#endif /* INC_SMAL_U8QUEUE_H */
