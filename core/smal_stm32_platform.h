#ifndef INC_SMAL_STM32_PLATFORM_H
#define INC_SMAL_STM32_PLATFORM_H

#pragma once

#include <stddef.h>
#include <stdint.h>

extern const volatile uint64_t *jiffies_p;

#if defined (__cplusplus )
extern "C" {
#endif

char smal_stm32_platform_uart2_getc(void);
void smal_stm32_platform_uart2_putc(const int ch);
void smal_stm32_platform_uart2_puts(const char *const str);

void smal_stm32_platform_init(void *const virtual_heal_buf, const uint32_t bufsz);

int smal_stm32_platform_uart_gets( char * const recvbuf, const size_t recvbufsz);
void smal_stm32_platform_uart_set_echoback_to_on(const int is_true);
int smal_stm32_platform_uart_get_echoback_is_on(void);

void smal_stm32_platform_interval_timer_add_service_function(const uint32_t interval_msec, void(* const cb_func)(void *const), void * const args_p);

void smal_stm32_platform_interval_timer_service_exec_funcs(void);

void smal_stm32_platform_delay_ms(const int milisec);

void smal_stm32_platform_disable_interrupts(void);
void smal_stm32_platform_enable_interrupts(void);

uint32_t smal_stm32_platform_read_register32(const uintptr_t addr);
void smal_stm32_platform_write_register32( const uintptr_t addr, const uint32_t data);

#if defined (__cplusplus )
}
#endif

#endif /* end of INC_SMAL_STM32_PLATFORM_H */
