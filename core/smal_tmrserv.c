/**
 *	Copyright 2020 TSN-SHINGENN All Rights Reserved.
 *	Basic Author: Seiichi Takeda  '2020-December-10 Active
 *
 *	Dual License :
 *	non-commercial ... MIT Licence
 *	    commercial ... Requires permission from the author
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* debug */
#ifdef __GNUC__
__attribute__ ((unused))
#endif
#ifdef DEBUG
static int debuglevel = 5;
#else
static const int debuglevel = 0;
#endif
#include "dbms.h"

#define EOL_CRLF "\n\r"

#include "smal_malloc.h"
#include "smal_slist.h"
#include "smal_tmrserv.h"

typedef struct {
    size_t interval_time_msec;
    size_t timer_counter_msec;
    void (*timer_cb_func)(void*);
    void *args_p;
} interval_timer_handler_t;

typedef struct {
    void (*isobj_func)(void*);
    void *args_p;
} isobj_handler_table_t;

typedef struct tmrserv_status {
    const volatile uint64_t *jiffies_p;
    smal_slist_t interval_timer_handler_queue;
    smal_slist_t itobj_handler_queue;
    union {
	uint8_t flags;
	struct {
	    uint8_t interval_timer_handler_queue:1;
	    uint8_t itobj_handler_queue:1;
	    uint8_t jiffies_p:1;
	} f;
    } init;
    union {
	uint8_t flags;
	struct {
	    uint8_t ilock:1;
	} f;
    } stat;
} tmrserv_status_t;

static tmrserv_status_t tmrserv_stat;

#define get_tmrserv_stat_p() (&tmrserv_stat)

void smal_tmrserv_init(const volatile uint64_t *const jiffies_p)
{
    tmrserv_status_t * const s = get_tmrserv_stat_p();
    int result;

    s->jiffies_p = jiffies_p;
    result = smal_slist_init( &s->interval_timer_handler_queue, sizeof(interval_timer_handler_t));
    if(result) {
	DBMS1( "%s :  smal_slist_init(interval_timer_handler_queue) fail : %s" EOL_CRLF,
		__func__, strerror(result));
	abort();
    }

    result = smal_slist_init( &s->itobj_handler_queue, sizeof(isobj_handler_table_t));
    if(result) {
	DBMS1( "%s :  smal_slist_init(itobj_handler_queue) fail : %s" EOL_CRLF,
		__func__, strerror(result));
	abort();
    }
    s->init.f.jiffies_p = 0x1;
    s->init.f.interval_timer_handler_queue = 0x1;
    s->init.f.itobj_handler_queue = 0x1;

    return;
}

/**
 * @fn void smal_tmrserv_add_cbfunc(const uint32_t interval_msec, void(* const cb_func)(void *const), void * const args_p)
 * @brief ソフトウェアタイマーで呼び出る関数を追加します。
 */
void smal_tmrserv_add_cbfunc(const uint32_t interval_msec, void(* const cb_func)(void *const), void * const args_p)
{
    tmrserv_status_t * const s = get_tmrserv_stat_p();
    int result;

    if(s->init.flags) {
	static interval_timer_handler_t ith_tab;


	ith_tab.interval_time_msec = interval_msec;
	ith_tab.timer_counter_msec = 0;
	ith_tab.timer_cb_func = cb_func;
	ith_tab.args_p = args_p;

	while(s->stat.f.ilock);
	result = smal_slist_push( &s->interval_timer_handler_queue, &ith_tab, sizeof(interval_timer_handler_t));
	if( result ) {
	    DBMS1( "%s : smal_slist_back(intr_tab) fail, stderror=%s\n",
			__func__, strerror(result));
	    abort();
	}
    }

    return;
}

/**
 * @fn void smal_tmrserv_systick_callback_handler(void)
 * @brief 1msタイミングでコールすると、呼び出すべき関数を準備します。
 */
void smal_tmrserv_systick_callback_handler(void)
{
    tmrserv_status_t * const s = get_tmrserv_stat_p();
    static size_t n;
    static int result;

    if(s->init.flags) {

	const size_t num_funcs = smal_slist_get_pool_cnt(&s->interval_timer_handler_queue);
	static uint64_t old_jiffies=0;
	static uint64_t intr_laps;

	/* 前回呼び出された時間からのラップタイムを計算 */
	intr_laps = (*s->jiffies_p == 0) ? 0 : (*s->jiffies_p - old_jiffies);
	if(intr_laps) {
	    old_jiffies = *s->jiffies_p;	    
	} else {
	    DBMS5( "%s : jiffies not incliment" EOL_CRLF, __func__);
	    return;
	}
	if(!num_funcs) {
	    return;
	}

	s->stat.f.ilock = 1; /* Lock */

	for(n=0; n<num_funcs; ++n) {
	    static interval_timer_handler_t ith_tab;

	    result = smal_slist_get_element_at( &s->interval_timer_handler_queue, n, &ith_tab, sizeof(ith_tab));
	    if(result) {
		DBMS5("%s : smal_slist_get_element_at(%lld) fail, %s" EOL_CRLF, __func__, n, strerror(result));
		abort();
	    }
	    /* 呼ばれたタイミング情報を更新 */
	    ith_tab.timer_counter_msec += intr_laps; 

	    /* 閾値を超えていたら　itobj_handler_queueにリストする */
	    if(ith_tab.timer_counter_msec >= ith_tab.interval_time_msec) {	
		static isobj_handler_table_t obj_tab;

		obj_tab.isobj_func = ith_tab.timer_cb_func;
                obj_tab.args_p = ith_tab.args_p;

		result = smal_slist_push( &s->itobj_handler_queue, &obj_tab, sizeof(obj_tab));
		if(result) {
		    DBMS5("%s : smal_slist_push fail, %s" EOL_CRLF, __func__, n, strerror(result));
		    abort();
		}
		/* 呼び出し時間初期化 */
		(ith_tab.timer_counter_msec) = 0;
	    }
	    result = smal_slist_overwrite_element_at(
			    &(s->interval_timer_handler_queue), n, &ith_tab, sizeof(ith_tab));

	    if(result) {
		DBMS5("%s : smal_slist_overwrite_element_at(%lld) fail,%s" EOL_CRLF,
			__func__, n, strerror(result));
		abort();
	    }
	}
	s->stat.f.ilock = 0; /* Unlock */
    }
}

/**
 * @fn void smal_tmrserv_exec_funcs(void)
 * @brief smal_tmrserv_systick_callback_handler()で準備された関数を実行します。
 **/
void smal_tmrserv_exec_funcs(void)
{

    tmrserv_status_t * const s = get_tmrserv_stat_p();

    if(s->init.flags) {
	static size_t n;
	static int result;
	const size_t num_funcs = smal_slist_get_pool_cnt(&s->itobj_handler_queue);

	while(s->stat.f.ilock);
	if(num_funcs) {
	    isobj_handler_table_t tab;

	    for( n=0; n<num_funcs; ++n) {
		result =  smal_slist_get_element_at( &s->itobj_handler_queue, n, &tab, sizeof(tab));
		if(result) {
		    DBMS1("%s :  smal_slist_get_element_at(%d) fail,%s" EOL_CRLF,
				__func__, (int)n, strerror(result));
		    abort();
		}
		result = smal_slist_pop( &s->itobj_handler_queue);
		if(result) {
		    DBMS1("%s :  multios_stl_list_pop_front(%d) fail,%s" EOL_CRLF,
				__func__, (int)n, strerror(result));
		    abort();
		}

		if( NULL != tab.isobj_func ) {
		    tab.isobj_func(tab.args_p);
		    // check_interrput_mismatch("mbmcs_platform_interval_timer_service_exec_funcs callback proc");
		}
	    }
	}
    }
}


/**
 * @fn void smal_tmrserv_delay_ms(const int milisec)
 * @brief jiffiesを参照した遅延関数
 **/
void smal_tmrserv_delay_ms(const int milisec)
{
    tmrserv_status_t * const s = get_tmrserv_stat_p();
    const uint64_t now_jiffies = *(s->jiffies_p);
    const uint64_t endtime = now_jiffies + milisec;

    while(endtime > *(s->jiffies_p)) {
	smal_tmrserv_exec_funcs();
    }
}
