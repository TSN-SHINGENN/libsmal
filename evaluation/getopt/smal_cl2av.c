/**
 *      Copyright 2020 TSNｰSHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-July-02 Active
 *              Last Alteration $Author: takeda $
 */

/**
 * @file smal_cl2av.cpp
 *   コマンドライン引数分割ライブラリ
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
// #include <stdlib.h>
#include <string.h>

/* this */
#include "smal_cl2av.h"

typedef struct _own_strtok {
    char *strtok_pbuf;
    smal_cl2av_attr_t attr;

    union {
	unsigned int flags;
	struct {
	    unsigned int initial:1;
	    unsigned int final:1;
	} f;
    } stat;
} own_strtok_t;


static own_strtok_t own_strtok_reset(const smal_cl2av_attr_t attr);

static int own_strtok(own_strtok_t * const __restrict self_p,
		      char **result_pointer, char *s, const char *delim,
		      const char delim2);
static int own_strtok_is_eol(own_strtok_t * const __restrict self_p, const char*const delim);

/**
 * @fn static own_strtok_t own_strtok_reset( const smal_cl2av_attr_t attr)
 * @brief own_strtokオブジェクトを初期化します
 * @return 初期化されたown_strtok_tインスタンス
 */
static own_strtok_t own_strtok_reset( const smal_cl2av_attr_t attr)
{
    own_strtok_t ret;

    ret.strtok_pbuf = NULL;
    ret.attr = attr;
    ret.stat.flags = 0;

    return ret;
}

/**
 * @fn static int own_strtok( own_strtok_t *self_p, char **result_pointer, char *s, const char *delim, const char delim2)
 * @brief デリミタを認識してNULLで区切りそのトークンとします。次のトークンのポインタを返します。
 *	delim2で囲まれた部分はそれを一つのトークンとして区切ります
 * @param self_p own_strtokインスタンスポインタ
 * @param result_pointer 次のトークンへのポインタまた、 次のトークンがなければ NULL を返す。ポインタ
 * @param s 解析対象の文字列バッファポインタ(二個目以降のトークンの解析にはNULLを指定します）
 *	指定されたバッファーはトークンに分割するためデータを変更します
 * @param delim デリミタのASCII文字集合列
 * @param delim2 囲い文字ASCII文字('\0'で無視)
 * @retval 0 成功
 * @retval EINVAL 引数が不正
 * @retval EIO 囲い文字の終端が見つからない
 */
static int own_strtok(own_strtok_t * const __restrict self_p,
		      char **result_pointer, char *s, const char *delim,
		      const char delim2)
{
    int status;
    enum _enum_fence_mode {
	IN_FENCE,
	NOT_FENCE
    } mode;
    size_t n, remain_length;
    char *delim_point, *delim2_point;

    /* inital */
    delim_point = delim2_point = NULL;

    if (!self_p->stat.f.initial) {
	if (NULL == s) {
	    return EINVAL;
	}
	self_p->strtok_pbuf = s;
	self_p->stat.f.initial = 1;
	if((delim2 != '\0') && (NULL!=strchr(delim, delim2))) {
	    /* 囲い文字とデリミタが重なっている */
  	    return EINVAL;
	}
    } 

    /* 囲い文字が設定されていて、先頭から囲い文字が出てきた場合は先方の区切り文字飛ばしは行わない */
    if(('\0' != *self_p->strtok_pbuf) && (delim2 != *self_p->strtok_pbuf)) {
	char * __restrict p = self_p->strtok_pbuf;
	int a, b, c;

	/* 先頭から終端文字・囲い文字 ではなく、区切り文字の場合はインクリメントして飛ばす */
	while(((a=('\0' != *p)) && (b=( delim2 != *p ))) && (c=(NULL != strchr(delim, *p)))) {
	    /* 空白文字の扱いが特殊なので検出時は脱出する */
	    if (!(self_p->attr.f.untrim_space) && (' ' == *p) ) {
		break;
	    }
	    ++p;
	}
	self_p->strtok_pbuf = p;

	/* すでに終端なら抜ける */
	if( '\0' == *(self_p->strtok_pbuf) ) {
	    *result_pointer = NULL;
	    status = 0;
	    goto out;
	}
    }

    /* 先頭のデリミタを飛ばしたら残りの文字数を数える */
    remain_length = strlen(self_p->strtok_pbuf);
    if (remain_length == 0) {
	*result_pointer = NULL;
	status = 0;
	goto out;
    }

    /* フラグが立ってた場合スペースを除外する */
    if (!(self_p->attr.f.untrim_space)) {
	for (n = 0; n < remain_length; ++n) {
	    if (delim2 == *self_p->strtok_pbuf) {
		break;
	    } else if (' ' != *(self_p->strtok_pbuf)) {
		break;
	    }

	    ++(self_p->strtok_pbuf);
	}
    } else {
	n = 0;
    }

    /* 次のデリミタの位置(トークンの終わり)を探す */
    delim_point = strpbrk(self_p->strtok_pbuf, delim);
    if ('\0' != delim2) {
	delim2_point = strchr(self_p->strtok_pbuf, delim2);
    }
    if ((NULL == delim_point) && (NULL == delim2_point)) {
	*result_pointer = self_p->strtok_pbuf;
	self_p->strtok_pbuf += (remain_length - n);	/* Trimした分を引く */
	status = 0;
	goto out;
    } else if ((delim_point == NULL)
	       || (delim_point == self_p->strtok_pbuf)
	       || ('\0' == *(self_p->strtok_pbuf))) {
	/* これ以上、トークンは無い */
	*result_pointer = NULL;
	status = 0;
	goto out;
    }

    if (NULL == delim2_point) {
	mode = NOT_FENCE;
    } else if (delim_point < delim2_point) {
	mode = NOT_FENCE;
    } else {
	mode = IN_FENCE;
    }

    switch (mode) {
    case NOT_FENCE:
	/* 囲い文字で囲まれてなかった場合 */
	*delim_point = '\0';
	*result_pointer = self_p->strtok_pbuf;
	self_p->strtok_pbuf = delim_point + 1;
	break;

    case IN_FENCE:
	/* 囲い文字で囲まれた場合 */

	/* 1.囲い文字の先頭を飛ばす */
	self_p->strtok_pbuf = delim2_point + 1;

	/* 2.囲い文字の終了を探す */
	delim2_point = strchr(self_p->strtok_pbuf, delim2);

	/* 3a. 見つからない場合 */
	if (NULL == delim2_point) {
	    /* エラーリターン */
	    status = EIO;
	    goto out;
	}

	/* 3b. 見つかった場合囲い文字を消す */

	*result_pointer = self_p->strtok_pbuf;
	*delim2_point = '\0';
	self_p->strtok_pbuf = delim2_point + 1;

	/* さらにトークンが続く場合は1文字トークンを飛ばす */
	if( '\0' != *self_p->strtok_pbuf ) {
	    delim_point = strchr((char *) delim, *self_p->strtok_pbuf);
	    if (NULL != delim_point) {
		++(self_p->strtok_pbuf);
	    }
	}
    }

    /* success */
    status = 0;
  out:
    if (status) {
	*result_pointer = NULL;
    }

    return status;
}

/**
 * @fn static int own_strtok_is_eol(own_strtok_t * const __restrict self_p, const char*const delim)
 * @brief 解析処理中の後方にトークンで分割する必要があるかどうかを確認します。
 * @param self_p own_strtokインスタンスポインタ
 * @param トークン分割するための文字種
 * @retval 0 後方も解析する必要有り
 * @retval 0以外 後方はすでに解析する必要なし(終端に達している)
 */
static int own_strtok_is_eol(own_strtok_t * const __restrict self_p, const char*const delim)
{

    if(NULL != self_p->strtok_pbuf) {
	const size_t len = strlen(self_p->strtok_pbuf);
	if(!len) {
	     return 1;
	} else { 
	     char *p = self_p->strtok_pbuf;
	     size_t l;
	     for(l=0; l<len; ++p, ++l) {
	 	if(NULL == strrchr(delim, (int)*p)) {
		      break;
		}
	     }
	     return ( l==len ) ? 1 : 0;
	}
    }

    return 0;
}

/**
 * @fn int smal_cl2av_lite( char * const __restrict str, const char * __restrict delim, char delim2, int *const __restrict argc_p, char ** __restrict argv_p, const unsigned int limit, smal_cl2av_attr_t *const attr_p )
 * @brief 文字列を解析して要素に分割します。オブジェクトのいらないlite版です
 *	第一引数strの文字列は解析時に書き換えられます。
 * @param str 要素ごとに分割する文字列バッファです。分割後にバッファ内容を書き換えます
 * @param delim デリミタのASCII文字集合列( NULL で " "と等価 )
 * @param delim2 囲い文字ASCII文字('\0'で無視)
 * @param argc 要素数を返すための変数ポインタ
 * @param argv 要素に分割された文字列のポインタ集合を返します。配列数は次の引数limit分確保してください
 * @param limit 要素分割の最大数を返します。 argv配列を超えて、バッファーオーバランを起こさないためのリミッタです。
 * @retval 0 成功
 * @retval EINVAL 引数が不正
 * @retval ERANGE limitを超えて引数解析をしようとした
 **/
int smal_cl2av_lite( char * const __restrict str, const char * __restrict delim, char delim2, int *const __restrict argc_p, char ** __restrict argv_p, const unsigned int limit, smal_cl2av_attr_t *const attr_p )
{
    int result;
    unsigned int n;    
    smal_cl2av_attr_t attr;
    own_strtok_t s; 
    char *buf = str;

    if( (NULL == str) || ( NULL == argv_p ) || ( NULL == argc_p ) || ( limit == 0 ) ) {
	return EINVAL;
    }

    if (NULL == delim) {
	    char *const std_delim = " ";
	delim = std_delim;
    }

    if( NULL != attr_p ) {
	attr = *attr_p;
    } else {
	attr.flags = 0;
    }
    s = own_strtok_reset(attr);
    
    for( n=0; n<limit; ++n) {
	char *argp;
	result = own_strtok( &s, &argp, buf, delim, delim2);
	if(result) {
	    return result;
	}
	if( argp == NULL ) {
	    break;
	} else {
	    argv_p[n] = argp;
	}
    }

    if( n==limit && own_strtok_is_eol(&s, delim)) {
	return EOVERFLOW;
    }

    *argc_p = n;

    return 0;
}

