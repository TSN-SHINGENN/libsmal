/**
 *	Copyright 2020 TSN-SHINGENN All Rights Reserved.
 *	Basic Author: Seiichi Takeda  '2020-November-03 Active
 */

/**
 * @file smal_u8queue.c
 * @brief u8キュー（リングバッファ）ライブラリ
 **/

#include <stddef.h>
#include <stdio.h>
#include <errno.h>

#include "smal_malloc.h"
#include "smal_u8queue.h"

/* キューの次の挿入位置を求める */
#define queue_next(s, n) (((n) + 1) % (s)->queue_size)

int smal_u8queue_init(smal_u8queue_t *const self_p, const uint8_t size)
{
    self_p->head = 0;
    self_p->tail = 0;
    self_p->buf = (uint8_t*)smal_malloc(sizeof(uint8_t)*size);
    if (NULL == self_p->buf) {
	return ERANGE;
    }
    self_p->queue_size = size;

    return 0; 
}

int smal_u8queue_is_empty(smal_u8queue_t *const self_p)
{
    return (self_p->head == self_p->tail);
}

int smal_u8queue_push(smal_u8queue_t *const self_p, const uint8_t u8)
{
    /* キューが満杯であるか確認する */
    if(queue_next(self_p, self_p->tail) == self_p->head) {
	return -1;
    }

    /* キューの末尾にデータを挿入する */
    self_p->buf[self_p->tail] = u8;

    /* キューの次回挿入位置を決定にする */
    self_p->tail = queue_next(self_p, self_p->tail);

    return 0;
}

int smal_u8queue_pop(smal_u8queue_t *const self_p, uint8_t *const u8_p)
{
    /* キューに取り出すデータが存在するか確認する */
    if(smal_u8queue_is_empty(self_p)) {
	return -1;
    }

    /* キューからデータを取得する */
    *u8_p = self_p->buf[self_p->head];

    /* 次のデータ取得位置を決定する */
    self_p->head = queue_next(self_p, self_p->head);

    return 0;
}

int smal_u8queue_destroy(smal_u8queue_t *const self_p)
{
    smal_free(self_p->buf);
    return 0;
}
