
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void smal_tmrserv_init(const volatile uint64_t *const jiffies_p);
void smal_tmrserv_add_cbfunc(const uint32_t interval_msec, void(* const cb_func)(void *const), void * const args_p);

void smal_tmrserv_systick_callback_handler(void);

void smal_tmrserv_exec_funcs(void);
void smal_tmrserv_delay_ms(const int milisec);

#ifdef __cplusplus
}
#endif

