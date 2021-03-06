/**
 *	Copyright 2020 TSN-SHINGENN All Rights Reserved.
 *	Basic Author: Seiichi Takeda  '2014-March-01 Active
 */

/**
 * @file smal_slist.c
 * @brief 単方向待ち行列ライブラリ STLのslist(SGI)クラス互換です。
 *	なのでスレッドセーフではありません。上位層で処理を行ってください。
 *	STL互換と言ってもCの実装です、オブジェクトの破棄の処理はしっかり実装してください
 */

/* CRL */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* this */
#include "smal_mallocater.h"
#include "smal_slist.h"

typedef struct _fifoitem {
    struct _fifoitem *next;
    uint8_t data[0];
} fifoitem_t;

typedef struct _slist_ext {
    fifoitem_t *r_p, *w_p;	/* カレント参照のポインタ */
    uint8_t sizof_element;
    unsigned int cnt;
    fifoitem_t base;		/* 配列0の構造体があるので必ず最後にする */
} slist_ext_t;

#define get_slist_ext(s) (slist_ext_t*)((s)->ext)
#define get_const_slist_ext(s) (const slist_ext_t*)((s)->ext)

static fifoitem_t *slist_get_foward_element_ptr( const slist_ext_t *const e, void * const element_ptr);

/**
 * @fn static fifoitem_t *slist_get_foward_element_ptr( const stl_slist_ext_t *const e, void * const element_ptr)
 * @brief 指定されたエレメントバッファの前方のエレメントバッファのポインタを返します。
 *	外部関数でinsert, eraseを処理する為に使用します。
 * @param stl_slist_ext_t構造体ポインタ
 * @param element_ptr 基準とするエレメントポインタ
 * @retval NULL 前方のポインタは無い
 * @retval NULL以外 前方のfifoエレメントポインタ
 */
static fifoitem_t *slist_get_foward_element_ptr( const slist_ext_t *const e, void * const element_ptr)
{
    fifoitem_t *cur;

    if( (e->cnt == 0) || (e->w_p == &e->base) ) {
	return NULL;
    }

    for( cur=e->w_p; cur->next != NULL; cur=cur->next) {
	if( cur->next == element_ptr ) {
	    return cur;
	}
    }

    return NULL;
}


/**
 * @fn int smal_slist_init( smal_slist_t *const self_p, const uint8_t sizof_element, void *const heapbuf, const uint32_t bufsz)
 * @brief オブジェクトを初期化します
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @param sizof_element エレメントのサイズ
 * @retval 0 成功
 * @retval EAGAIN リソースの獲得に失敗
 * @retval -1 それ以外の致命的な失敗
 */
int smal_slist_init( smal_slist_t *const self_p, const uint8_t sizof_element, void *const heapbuf, const uint32_t bufsz)
{
    slist_ext_t *e = NULL;

    smal_mallocater_init_obj(&self_p->mem, heapbuf, bufsz);

    e = (slist_ext_t *)smal_mallocater_alloc_with_obj(&self_p->mem, sizeof(slist_ext_t));
    if (NULL == e) {
	return ERANGE;
    }

    self_p->ext = e;

    e->sizof_element = sizof_element;

    e->r_p = e->w_p = &e->base;

    return 0;
}

/**
 * @fn int smal_slist_destroy( smal_slist_t *const self_p)
 * @brief キューオブジェクトを破棄します
 * @param self_p smal_slist_t構造体インスタンスポインタ
 * @retval -1 errno.h以外の致命的な失敗
 * @retval 0 成功
 */
int smal_slist_destroy( smal_slist_t *const self_p)
{
    int result;
    slist_ext_t * const e = get_slist_ext(self_p);

    result = smal_slist_clear(self_p);
    if (result) {
	return -1;
    }

    /* 最後のエレメントがbaseで無ければ削除 */
    if (e->r_p != &e->base) {
	smal_mallocater_free_with_obj( &self_p->mem, e->r_p);
	e->r_p = NULL;
    }

    smal_mallocater_free_with_obj( &self_p->mem, self_p->ext);

    return 0;
}

/**
 * @fn int smal_slist_push(smal_slist_t *const self_p, const void *const el_p, const uint8_t sizof_element)
 * @brief キューにエレメントを追加します
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @param el_p エレメントポインタ
 * @param sizof_element エレメントサイズ(mpsx_stl_slist_initで指定した以外のサイズはエラーとします)
 * @retval 0 成功
 * @retval EINVAL エレメントサイズが異なる
 * @retval EAGAIN リソースの獲得に失敗
 * @retval -1 それ以外の致命的な失敗
 */
int smal_slist_push(smal_slist_t *const self_p, const void *const el_p, const uint8_t sizof_element)
{
    slist_ext_t * const e = get_slist_ext(self_p);
    fifoitem_t *__restrict f;

    if (e->sizof_element != sizof_element) {
	return EINVAL;
    }

    f = (fifoitem_t *)smal_mallocater_alloc_with_obj(&self_p->mem, (sizeof(fifoitem_t) + e->sizof_element));
    if (NULL == f) {
	return EINVAL;
    }

    f->next = NULL;
    memcpy(f->data, el_p, e->sizof_element);

    /* キューに追加 */
    e->w_p->next = f;
    e->w_p = f;
    ++(e->cnt);

    return 0;
}

/**
 * @fn int smal_slist_pop(smal_slist_t *const self_p)
 * @brief キューの先頭の要素を削除します
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @retval ENOENT 削除するエレメントが存在しない
 * @retval -1 それ以外の致命的な失敗
 * @retval 0 成功
 */
int smal_slist_pop(smal_slist_t *const self_p)
{
    slist_ext_t * const e = get_slist_ext(self_p);
    fifoitem_t *__restrict tmp = NULL;

    /* エレメントがあるかどうか二重チェック */
    if (e->cnt == 0 ) {
	return ENOENT;
    }

    if((e->r_p == &e->base) && ( e->r_p->next == NULL)) { // ここの検査がよく分からないので調べる必要有り
	return -1;
    }

    tmp = e->r_p;
    e->r_p = e->r_p->next;
    --(e->cnt);

    if (tmp != &e->base) {
	smal_mallocater_free_with_obj(&self_p->mem, tmp);
    }

    if (e->cnt == 0) {
	if (e->r_p != &e->base) {
	    smal_mallocater_free_with_obj(&self_p->mem, e->r_p);
	}
	e->r_p = e->w_p = &e->base;
    }

    return 0;
}

/**
 * @fn int smal_slist_front(smal_slist_t *const self_p, void *const el_p, const uint8_t sizof_element)
 * @brief キューの先頭のエレメントデータを返します
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @param el_p エレメントデータ取得用データバッファポインタ
 * @param sizof_element エレメントサイズ(mpsx_stl_slist_initで指定した以外のサイズはエラーとします)
 * @retval EINVAL  エレメントサイズが異なる
 * @retval ENOENT エレメントが存在しない
 * @retval -1 それ以外の致命的な失敗
 * @retval 0 成功
 */
int smal_slist_front(smal_slist_t *const self_p, void *const el_p, const uint8_t sizof_element)
{
    slist_ext_t * const e = get_slist_ext(self_p);
    fifoitem_t *__restrict f;

    if (e->sizof_element != sizof_element) {
	return EINVAL;
    }

    if (e->cnt == 0) {
	/* キューにエレメントが無い場合 */
	return EINVAL;
    }
    f = e->r_p->next;

    memcpy(el_p, f->data, e->sizof_element);

    return 0;
}

/**
 * @fn int smal_slist_clear(smal_slist_t *const self_p)
 * @brief キューに貯まっているエレメントデータを全て破棄します
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @retval -1 致命的な失敗
 * @retval 0 成功
 */
int smal_slist_clear(smal_slist_t *const self_p)
{
    slist_ext_t * const e = get_slist_ext(self_p);
    size_t n;
    int result;

    if (e->cnt == 0) {
	return 0;
    }

    for (n = e->cnt; n != 0; --n) {
	result = smal_slist_pop(self_p);
	if (result) {
	    return -1;
	}
    }

    return 0;
}

/**
 * @fn uint16_t smal_slist_get_pool_cnt(smal_slist_t *const self_p)
 * @brief キューにプールされているエレメント数を返します
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @retval 0以上 要素数
 */
uint16_t smal_slist_get_pool_cnt(smal_slist_t *const self_p)
{
    const slist_ext_t * const e = get_const_slist_ext(self_p);

    return e->cnt;
}

/**
 * @fn int smal_slist_is_empty(smal_slist_t *const self_p)
 * @brief キューが空かどうかを判定します
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @retval -1 失敗
 * @retval 0 キューは空ではない
 * @retval 1 キューは空である
 */
int smal_slist_is_empty(smal_slist_t *const self_p)
{
    const slist_ext_t * const e = get_const_slist_ext(self_p);

    return (e->cnt == 0) ? 1 : 0;
}

/**
 * @fn int mpsx_stl_slist_get_element_at( mpsx_stl_slist_t *const self_p, const unsigned int num, void *const el_p, const int sizof_element);
 * @brief キューに保存されているエレメントを返します
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @param num 0空始まるキュー先頭からのエレメント配列番号
 * @param el_p エレメントデータコピー用エレメント構造体ポインタ
 * @param sizof_element エレメントサイズ(主にエレメントサイズ検証向け)
 * @retval 0 成功 失敗
 * @retval EACCES キューにエレメントがない
 * @retval ENOENT 指定されたエレメントが無い
 * @retval EFAULT el_pに指定されたアドレスがNULLだった
 * @retval EINVAL sizof_elementのサイズが異なる(小さい）
 */
int smal_slist_get_element_at(smal_slist_t *const self_p,
				       const unsigned int num, void *const el_p,
				       const uint8_t sizof_element)
{
    slist_ext_t * const e = get_slist_ext(self_p);
    fifoitem_t *__restrict item_p;
    size_t n;

    if (smal_slist_is_empty(self_p)) {
	return EINVAL;
    }

    if (NULL == el_p) {
	return -1;
    }

    if (sizof_element < e->sizof_element) {
	return EINVAL;
    }

    if (!(num < e->cnt)) {
	return EINVAL;
    }

    for (item_p = &e->base, n = 0; n < num; ++n) {
	item_p = item_p->next;
    }

    memcpy(el_p, item_p->next->data, e->sizof_element);

    return 0;
}


/**
 * @fn int smal_slist_back( smal_slist_t *const self_p, void *const el_p, const uint8_t  sizof_element )
 * @brief キューの最後尾のエレメントデータを返します
 * @param self_p smal_slist_t構造体インスタンスポインタ
 * @param el_p エレメントデータ取得用データバッファポインタ
 * @param sizof_element エレメントサイズ(mpsx_stl_slist_initで指定した以外のサイズはエラーとします)
 * @retval EINVAL  エレメントサイズが異なる
 * @retval ENOENT エレメントが存在しない
 * @retval -1 それ以外の致命的な失敗
 * @retval 0 成功
 */
int smal_slist_back( smal_slist_t *const self_p, void *const el_p, const uint8_t  sizof_element )
{
    slist_ext_t * const e = get_slist_ext(self_p);
    fifoitem_t *__restrict tmp = NULL;

    /* エレメントがあるかどうか二重チェック */
    if (e->cnt == 0) {
	return ENOENT;
    } else if( e->sizof_element != sizof_element ) {
	return EINVAL;
    }

    if (e->w_p == NULL) {
	return -1;
    }

    tmp = e->w_p;
    memcpy( el_p, tmp->data, e->sizof_element);

    return 0;
}

/**
 * @fn int smal_slist_insert_at( smal_slist_t *const self_p, const uint16_t no, void *const el_p, const uint8_t sizof_element)
 * @brief 指定されたエレメント番号の前に、エレメントデータを挿入します。
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @param no オブジェクト内の0〜のリストエレメント番号
 * @param el_p エレメントデータ取得用データバッファポインタ
 * @param sizof_element エレメントサイズ(mpsx_stl_slist_initで指定した以外のサイズはエラーとします)
 * @retval EINVAL  エレメントサイズが異なる
 * @retval ENOENT 指定された番号のエレメントが存在しない
 * @retval EAGAIN メモリ不足で追加不可能
 * @retval -1 それ以外の致命的な失敗
 * @retval 0 成功
 */
int smal_slist_insert_at( smal_slist_t *const self_p, const unsigned int no, void *const el_p, const uint8_t sizof_element)
{
    unsigned int n;
    slist_ext_t * const e = get_slist_ext(self_p);
    fifoitem_t *__restrict cur_ticket_p = &e->base;
    fifoitem_t *__restrict fwd_ticket_p;
    fifoitem_t *__restrict f;

    if (smal_slist_is_empty(self_p)) {
	return EINVAL;
    }

    if ((e->sizof_element != sizof_element) || ( no < e->cnt) ) {
	return EINVAL;
    }

    for (n = 0; n <= no; ++n) {
	cur_ticket_p = cur_ticket_p->next;
    }

    fwd_ticket_p = slist_get_foward_element_ptr( e, cur_ticket_p);
    if( NULL == fwd_ticket_p ) {
	return -1;
    }

    f = (fifoitem_t *) smal_mallocater_alloc_with_obj(&self_p->mem, sizeof(fifoitem_t) + e->sizof_element);
    if (NULL == f) {
	return -1;
    }

    memset(f, 0x0, sizeof(fifoitem_t));
    memcpy(f->data, el_p, e->sizof_element);
    f->next = NULL;

    /* キューに追加 */
    f->next = cur_ticket_p;
    fwd_ticket_p->next = f;
    ++(e->cnt);

    return 0;
}

/**
 * @fn int smal_slist_erase_at( smal_slist_t *const self_p, const unsigned int no)
 * @brief 指定されたエレメントを消去します。
 *      制御確認中
 * @param self_p mpsx_stl_slist_t構造体インスタンスポインタ
 * @param no オブジェクト内の0〜のリストエレメント番号
 * @retval 0 成功
 * @retval EINVAL 指定された番号のエレメントが存在しない
 **/
int smal_slist_erase_at( smal_slist_t *const self_p, const unsigned int no)
{
    slist_ext_t * const e = get_slist_ext(self_p);
    fifoitem_t *__restrict tmp = NULL;
    fifoitem_t *__restrict fwd_ticket_p;

    /* エレメントがあるかどうか二重チェック */
    if (e->cnt == 0 || !(no < e->cnt) ) {
	return EINVAL;
    }

    if((e->r_p == &e->base) || ( e->r_p->next == NULL)) {
	return EINVAL;
    }

    if( no == 0 ) {
	tmp = e->r_p;
	e->r_p = e->r_p->next;
	--(e->cnt);
    } else {
	size_t n;
	fifoitem_t *__restrict cur_ticket_p = &e->base;

	for (n = 0; n <= no; ++n) {
            cur_ticket_p = cur_ticket_p->next;
	}

        fwd_ticket_p = slist_get_foward_element_ptr( e, cur_ticket_p);
        if( NULL == fwd_ticket_p ) {
	    return -1;
        }
    }

    if (tmp != &e->base) {
	 smal_mallocater_free_with_obj(&self_p->mem, tmp);
    }

    if (e->cnt == 0) {
	if (e->r_p != &e->base) {
	    smal_mallocater_free_with_obj(&self_p->mem, e->r_p);
	}
	e->r_p = e->w_p = &e->base;
    }

    return 0;

}
