/**
 *      Copyright 2020 TSNｰSHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-July-02 Active
 *              Last Alteration $Author: takeda $
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* this */
#include "smal_mallocater.h"
#include "smal_malloc.h"

smal_mallocater_t _smal_mallocater_heap_obj;

#define get_lite_allocater_own() &(_smal_mallocater_heap_obj)

/**
 * @fn void smal_malloc_init(void *const buf, const uint32_t bufsize)
 * @brief mallocを扱えるように初期化します。
 * @param buf バッファポインタ
 * @param bufsize　バッファサイズ
 **/
void smal_malloc_init(void *const buf, const uint32_t bufsize)
{
    smal_mallocater_init_obj(&_smal_mallocater_heap_obj, buf, bufsize);
}


/**
 * @fn void *smal_malloc(const uint8_t size)
 * @brief CRL allocをエミュレートするためのラッパ関数
 * @param size 割り当てるメモリバッファサイズ
 * @retval NULL 割り当て失敗（errno番号参照）
 *	smal_mallocater_alloc_with_obj()参照
 * @retval NULL以外 割り当て成功
 **/
void *smal_malloc(const uint8_t size)
{
    // smal_mallocater_t * const o = get_lite_allocater_own();

    return smal_mallocater_alloc_with_obj( /* o */ get_lite_allocater_own(), size);
}

/**
 * @fn void smal_free(void *const ptr)
 * @brief CRL freeをエミュレートするためのラッパ関数
 *  ptrがNULL以外で、割り当て管理されていない場合はabort()を実行します
 * @param ptr 割り当てられたバッファのポインタ
 **/
void smal_free(void *const ptr)
{
    // smal_mallocater_t * const o = get_lite_allocater_own();

    smal_mallocater_free_with_obj( /* o */ get_lite_allocater_own(), ptr);
}

#if 0
/**
 * @fn void *smal_realloc(void *const ptr, const size_t size)
 * @brief CRL reallocをエミュレートするためのラッパ関数
 *  ptrがNULL指定の場合はmpsx_lite_mallocater_alloc()と同等の処理をします。
 *　リサイズ前のバッファがオーバランしていれば，abort()処理を実行します。
 * @param ptr リサイズ前のバッファ
 * @param size リサイズ後のサイズ
 * @retval NULL 割り当て失敗(errno参照)
 * @retval NULL以外 リサイズしたバッファポインタ（ptrと値が異なり場合は、データをコピー済）
 **/
void *smal_realloc(void *const ptr, const size_t size)
{
    smal_mallocater_t * const o = get_lite_allocater_own();

    return smal_mallocater_realloc_with_obj( o, ptr, size);
}
#endif
