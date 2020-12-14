#ifndef INC_SMAL_CL2AV_H
#define INC_SMAL_CL2AV_H

#pragma once

#include <errno.h>
#include <stdint.h>

typedef union _smal_cl2av_attr {
    unsigned int flags;			/*!< ビットフラグを0クリアする時に使用 */
    struct {
	unsigned int untrim_space:1;	/*!< 空白文字を削除します */
    } f;
} smal_cl2av_attr_t; 

#if defined(__cplusplus)
extern "C" {
#endif

int smal_cl2av_lite( char * const __restrict str, const char * __restrict delim, char delim2, int *const __restrict argc_p, char ** __restrict argv_p, const unsigned int limit, smal_cl2av_attr_t *const attr_p );

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_SMAL_CL2AV */
