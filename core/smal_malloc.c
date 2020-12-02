/**
 *      Copyright 2020 TSN�SHINGENN All Rights Reserved.
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
 * @brief malloc��������悤�ɏ��������܂��B
 * @param buf �o�b�t�@�|�C���^
 * @param bufsize�@�o�b�t�@�T�C�Y
 **/
void smal_malloc_init(void *const buf, const uint32_t bufsize)
{
    smal_mallocater_init_obj(&_smal_mallocater_heap_obj, buf, bufsize);
}


/**
 * @fn void *smal_malloc(const uint8_t size)
 * @brief CRL alloc���G�~�����[�g���邽�߂̃��b�p�֐�
 * @param size ���蓖�Ă郁�����o�b�t�@�T�C�Y
 * @retval NULL ���蓖�Ď��s�ierrno�ԍ��Q�Ɓj
 *	smal_mallocater_alloc_with_obj()�Q��
 * @retval NULL�ȊO ���蓖�Đ���
 **/
void *smal_malloc(const uint8_t size)
{
    // smal_mallocater_t * const o = get_lite_allocater_own();

    return smal_mallocater_alloc_with_obj( /* o */ get_lite_allocater_own(), size);
}

/**
 * @fn void smal_free(void *const ptr)
 * @brief CRL free���G�~�����[�g���邽�߂̃��b�p�֐�
 *  ptr��NULL�ȊO�ŁA���蓖�ĊǗ�����Ă��Ȃ��ꍇ��abort()�����s���܂�
 * @param ptr ���蓖�Ă�ꂽ�o�b�t�@�̃|�C���^
 **/
void smal_free(void *const ptr)
{
    // smal_mallocater_t * const o = get_lite_allocater_own();

    smal_mallocater_free_with_obj( /* o */ get_lite_allocater_own(), ptr);
}

#if 0
/**
 * @fn void *smal_realloc(void *const ptr, const size_t size)
 * @brief CRL realloc���G�~�����[�g���邽�߂̃��b�p�֐�
 *  ptr��NULL�w��̏ꍇ��mpsx_lite_mallocater_alloc()�Ɠ����̏��������܂��B
 *�@���T�C�Y�O�̃o�b�t�@���I�[�o�������Ă���΁Cabort()���������s���܂��B
 * @param ptr ���T�C�Y�O�̃o�b�t�@
 * @param size ���T�C�Y��̃T�C�Y
 * @retval NULL ���蓖�Ď��s(errno�Q��)
 * @retval NULL�ȊO ���T�C�Y�����o�b�t�@�|�C���^�iptr�ƒl���قȂ�ꍇ�́A�f�[�^���R�s�[�ρj
 **/
void *smal_realloc(void *const ptr, const size_t size)
{
    smal_mallocater_t * const o = get_lite_allocater_own();

    return smal_mallocater_realloc_with_obj( o, ptr, size);
}
#endif
