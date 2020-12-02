#ifndef INC_SMAL_GETOPT_H
#define INC_SMAL_GETOPT_H

#pragma once

/**
 * @var enum_smal_getopt_long_has_arg_t
 * @brief オプションの引数を表す要素
 */
typedef enum _enum_smal_getopt_long_has_arg {
    SMAL_GETOPT_NO_ARGUMENT = 0,			/*!< オプションは引数をとらない */
    SMAL_GETOPT_REQUIRED_ARGUMENT = 1,		/*!< オプションは引数を必要とする */
    SMAL_GETOPT_OPTIONAL_ARGUMENT = 2,		/*!< オプションは引数をとってもとらなくても良い */
    SMAL_GETOPT_eot = -1				/*!< オプション配列終端用 */
} enum_smal_getopt_long_has_arg_t;

/**
 * @var smal_getopt_long_option_t
 * @brief smal_getopt_long()に渡すオプションキー
 * 	※ 配列の最後の要素は、全て NULLまたは0 で埋める
 */
typedef struct _smal_getopt_long_option {
    const char *name;				/*!< 長いオプションの名前 */
    enum_smal_getopt_long_has_arg_t has_arg; /*!< オプションの取り方 */
    int *flag_p;				/*!< NULLの場合val値を返す。
    						     NULL以外は関数に0を返し,このポインタの変数にval値を入する */
    int val;					/*!< flag がしめすポインタに代入する値 */
    const char *memo;				/*!< 使い方とかメモ(関数挙動に特に意味はない) */
} smal_getopt_long_option_t;

/**
 * @var smal_getopt_t
 * @brief smal_getopt????を利用する為のオブジェクト構造体
 */
typedef struct _smal_getopt {
    /* public */
    int argc;		/*!< 引数の数 */
    int optind;		/*!< 次に処理される要素のインデックス */
    int optopt;		/*!< エラーになったオプション文字コード */
    const char *optarg; /*!< オプションに引数がある場合の文字列の先頭ポインタ(ない場合はNULL) */

    /* private */
    int _sp;
} smal_getopt_t;

#if defined(__cplusplus)
extern "C" {
#endif

smal_getopt_t smal_getopt_initializer(void);

int smal_getopt(smal_getopt_t *const self_p, const int argc, const char *const argv[], const char *optstring);

int smal_getopt_long(smal_getopt_t *const self_p, const int argc, const char *const argv[], const char *const optstring, const smal_getopt_long_option_t *const opts, int *const index_p);

int smal_getopt_long_only(smal_getopt_t *const self_p, const int argc, const char *const argv[], const char *const optstring, const smal_getopt_long_option_t *const lopts, int *const index_p);

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_SMAL_GETOPT_H */
