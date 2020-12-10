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

/**
 * @note SysTick_Handler()を確認して、 HAL_SYSTICK_IRQHandler()が呼び出されていることを確認すること。
 **/

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "main.h"
#include "usart.h"

#include "smal_mallocater.h"
#include "smal_malloc.h"
#include "smal_tmrserv.h"
#include "smal_stm32_platform.h"

const volatile uint64_t *jiffies_p;

typedef struct _stm32_status {
    volatile uint64_t jiffies;

    union {
	uint8_t flags;
	struct {
	    uint8_t tmrserv:1;
	    uint8_t systick:1;
	    uint8_t malloc:1;
	} f;
    } init;
    union {
	uint8_t flags;
	struct {
	    uint8_t uart_echoback:1;
	} f;
    } stat;

} stm32_status_t;

static stm32_status_t stm32_stat;

#define get_stm32_stat_p() (&stm32_stat)


void HAL_SYSTICK_Callback(void)
{
    stm32_status_t * const s = get_stm32_stat_p();    

    ++s->jiffies;

    smal_tmrserv_systick_callback_handler();
}

static void systick_init(void)
{
    stm32_status_t * const s = get_stm32_stat_p();    

    s->jiffies = 0;

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

#if 0
    /* システムタイマーのソースを指定するが、CUDEIDEの支持で良いのではないか */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
#endif

    /* システムタイマーなのでプライオリティを上げる */
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    /* とりあえず一回読んでみる */
    HAL_SYSTICK_IRQHandler();
}

char smal_stm32_platform_uart2_getc(void)
{
    uint8_t ch=0;

    if(HAL_UART_Receive(&huart2, &ch, 1, 1)==HAL_TIMEOUT) {
	return 0;
    }

    return (ch==127) ? 0 : (char)ch;
}

void smal_stm32_platform_uart2_putc(const int ch)
{
    uint8_t u8 = 0xff & ch;

    HAL_UART_Transmit( &huart2, (uint8_t*)&u8, 1, 1000);
}

void smal_stm32_platform_uart2_puts(const char *const str)
{
    HAL_UART_Transmit( &huart2, (uint8_t*)str, strlen(str), 1000);
}



/**
 * @fn void smal_stm32_platform_init(void *const virtual_heal_buf, const uint32_t bufsz)
 */
void smal_stm32_platform_init(void *const virtual_heal_buf, const uint32_t bufsz)
{
    stm32_status_t * const s = get_stm32_stat_p();    

    jiffies_p = &(s->jiffies);

    smal_malloc_init(virtual_heal_buf, bufsz);
    s->init.f.malloc = 0x1;
    smal_tmrserv_init(jiffies_p);
    s->init.f.tmrserv = 0x1;
    systick_init();
    s->init.f.systick = 0x1;
}

uint64_t smal_stm32_get_jiffies(void)
{
    return *jiffies_p;
}


int smal_stm32_platform_uart_gets( char * const recvbuf, const size_t recvbufsz)
{
    stm32_status_t * const s = get_stm32_stat_p();    
    uint8_t *p = (uint8_t*)recvbuf;
    int len = 0;
    size_t remainsz = recvbufsz;

    if( remainsz < 2 ) {
	return -1;
    }

    memset( p, 0x0, remainsz);
    --remainsz;

    while(remainsz) {
        int result;
	char c;
	while((*p = smal_stm32_platform_uart2_getc()) == 0 ) {
	    /*　割り込み処理コールバック */
	    smal_stm32_platform_interval_timer_service_exec_funcs();
    	}

	if( *p == '\t') {
	    *p= ' ';
	}
	c = *p;
	switch(*p) {
	case '\r': // xil_printf("LF\r\n");
	case '\n': // xil_printf("CR\r\n");
	    ++len;
	case '\0': // xil_printf("null\r\n");
	    if(s->stat.f.uart_echoback) { smal_stm32_platform_uart2_puts("\r\n"); };
	    return len;
	case '\b': // xil_printf("BS\r\n");
	case 0x7f: // xil_printf("DEL\r\n");
	    if( len > 0 ) {
		--p;
		--len;
		if(s->stat.f.uart_echoback) { smal_stm32_platform_uart2_puts("\b \b"); };
	    }
	    break;
	default:
	    if(s->stat.f.uart_echoback) { smal_stm32_platform_uart2_putc(c); };
	    ++p;
	    ++len;
	    --remainsz;
	}
	// xil_printf("%d %d\r\n", '\n', '\0');
	// xil_printf("size:%d len:%d code=%d \r\n", size, len, (int)c);
    }	    

    return (len==0) ? -1 : len;

    





}

void smal_stm32_platform_uart_set_echoback_to_on(const int is_true)
{
    stm32_status_t * const s = get_stm32_stat_p();    

    s->stat.f.uart_echoback = (is_true) ? 0x1 : 0;
}
int smal_stm32_platform_uart_get_echoback_is_on(void)
{
    stm32_status_t * const s = get_stm32_stat_p();    

    return (s->stat.f.uart_echoback) ? ~0 : 0;
}

void smal_stm32_platform_interval_timer_add_service_function(const uint32_t interval_msec, void(* const cb_func)(void *const), void * const args_p)
{
    __disable_irq();
    smal_tmrserv_add_cbfunc(interval_msec, cb_func, args_p);
    __enable_irq();
}

void smal_stm32_platform_interval_timer_service_exec_funcs(void)
{
    __disable_irq();
    smal_tmrserv_exec_funcs();
    __enable_irq();
}

void smal_stm32_platform_disable_interrupts(void)
{
    __disable_irq();
}

void smal_stm32_platform_enable_interrupts(void)
{
    __enable_irq();
}

void smal_stm32_platform_delay_ms(const int milisec)
{
    const uint64_t now_jiffies = *jiffies_p;
    const uint64_t endtime = now_jiffies + milisec;

    while(endtime > *jiffies_p) {
	smal_stm32_platform_interval_timer_service_exec_funcs();
    }
}


uint32_t smal_stm32_platform_read_register32(const uintptr_t addr);
void smal_stm32_platform_write_register32( const uintptr_t addr, const uint32_t data);


