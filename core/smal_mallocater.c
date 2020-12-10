/**
 *      Copyright 2020 TSNｰSHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-July-02 Active
 *              Last Alteration $Author: takeda $
 *
 *	Dual License :
 *	non-commercial ... MIT Licence
 *	    commercial ... Requires permission from the author
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* this */
#include "smal_mallocater.h"

typedef struct __mallocate_area_header {
    struct {
	uint8_t pad:7;
	uint8_t is_used:1;
	uint8_t size;
    } stat;
} _mallocate_area_header_t;

/**
 * @fn void smal_mallocater_init_obj(smal_mallocater_t *const self_p, void *const buf, const uint32_t bufsize)
 * @brief オブジェクトを初期化します。
 */
void smal_mallocater_init_obj(smal_mallocater_t *const self_p, void *const buf, const uint32_t bufsize)
{
    self_p->buf = buf;
    self_p->bufsize = bufsize;
    self_p->remain = bufsize;
    self_p->fin = buf;

    memset( self_p->buf, 0x0, bufsize);

    return;
}

/**
 * @fn void *smal_mallocater_alloc_with_obj(smal_mallocater_t *const self_p,const uint8_t size)
 * @brief 先頭から空き領域を確認して空き領域を確保します。
 *	検索は線形でやります。時間はかかるが効率的にメモリを使用します。
 *	デフラグは無しよ
 */
void *smal_mallocater_alloc_with_obj(smal_mallocater_t *const self_p,const uint8_t size)
{
    static uint8_t *target;
    static uint32_t areasz;

    areasz = size + sizeof(_mallocate_area_header_t);
    if((size == 0) && (areasz > UINT8_MAX)) {
	return NULL;
    }
    target = NULL;

    if(self_p->remain == self_p->bufsize) {
	if(size > self_p->bufsize) {
	    /* 空きエリア不足 */
	    return NULL;
	}
	target = (uint8_t*)self_p->buf;
    } else {
	target=(uint8_t*)self_p->buf;
	if(self_p->fin != NULL) {
	    static uint32_t offset;
	    static uint32_t resvd;

	    /* initialize */
	    offset = 0;
	    resvd = self_p->bufsize - self_p->remain;
	    for(;(offset < resvd);) {
		_mallocate_area_header_t *h = (_mallocate_area_header_t*)target;

		if(!(h->stat.is_used) && (areasz == h->stat.size)) {
		    break;
		}
		offset += h->stat.size;
		target += h->stat.size;
	    }
	}
    }

    if((self_p->fin != NULL) && (target==(uint8_t*)self_p->fin)) {
	/* 最後に追加できるか */
	if(self_p->remain < size) {
	    return NULL;
	} else {
	    _mallocate_area_header_t *h = (_mallocate_area_header_t*)target;
	    self_p->remain -= areasz;
	    h->stat.is_used = 1;
	    h->stat.size= areasz;
	    if(self_p->remain < sizeof(_mallocate_area_header_t)) {
		self_p->fin = NULL; /* これ以上は確保無理 */
	    } else {   
		self_p->fin = (void*)((uint8_t*)h + areasz); /* 次のヘッダ開始位置 */
	    }
	    return (void*)((uint8_t*)h+sizeof(_mallocate_area_header_t));
	}
    } else {
	/* 途中に追加 */
	_mallocate_area_header_t *h = (_mallocate_area_header_t*)target;
	    
	h->stat.is_used = 1;
	return (void*)((uint8_t*)h+sizeof(_mallocate_area_header_t));
    }
}

/**
 * @fn void smal_mallocater_free_with_obj(smal_mallocater_t *const self_p, void * const ptr);
 * @brief 使用領域を解放します。フラグ解除だけです。
 */
void smal_mallocater_free_with_obj(smal_mallocater_t *const self_p, void * const ptr)
{
    static _mallocate_area_header_t *h;
   
    h = (_mallocate_area_header_t*)((uint8_t*)ptr - sizeof(_mallocate_area_header_t));

    h->stat.is_used = 0;

    return;
}

#if 0
void _smal_mallocater_dump_region_list(smal_mallocater_t const *const self_p)
{
    uint8_t *target = NULL;

    fprintf( stdout, "Object Infomation\n");
    fprintf( stdout, "self_p->bufsize=%u\n", self_p->bufsize);
    fprintf( stdout, "self_p->remain =%u\n", self_p->remain);
    fprintf( stdout, "self_p->fin    =%p\n", self_p->fin);

    if(self_p->fin != self_p->buf) {

	size_t n;
	uint32_t offset = 0;
	uint32_t resvd = self_p->bufsize - self_p->remain;
	target=(uint8_t*)self_p->buf;
	for( n=0;(offset < resvd); ++n) {
	    _mallocate_area_header_t *h = (_mallocate_area_header_t*)target;
	
	    fprintf( stdout, "[%u]offset=0x%x is_used=0x%x size=%u\n",
		(unsigned)n, (unsigned)offset,
		(unsigned)h->stat.is_used, (unsigned)h->stat.size);

	    offset += h->stat.size;
	    target += h->stat.size;
	}
    }

    return;
}
#endif
