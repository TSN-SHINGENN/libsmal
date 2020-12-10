/**
 *	Copyright 2020 TSN-SHINGENN All Rights Reserved.
 *	Basic Author: Seiichi Takeda  '2014-March-01 Active
 *
 *	Dual License :
 *	non-commercial ... MIT Licence
 *	    commercial ... Requires permission from the author
 */

/**
 * @file smal_slist.c
 * @brief �P�����҂��s�񃉃C�u���� STL��slist(SGI)�N���X�݊��ł��B
 *	�Ȃ̂ŃX���b�h�Z�[�t�ł͂���܂���B��ʑw�ŏ������s���Ă��������B
 *	STL�݊��ƌ����Ă�C�̎����ł��A�I�u�W�F�N�g�̔j���̏����͂�������������Ă�������
 */

/* CRL */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* this */
#include "smal_slist.h"

#define USE_SMAL_MALLOC

#ifdef USE_SMAL_MALLOC
#include "smal_malloc.h"
#define SMAL_MALLOC(s) smal_malloc((s))
#define SMAL_FREE(p) smal_free((p))
#else
#define SMAL_MALLOC(s) malloc((s))
#define SMAL_FREE(p) free((p))
#endif

typedef struct _fifoitem {
    struct _fifoitem *next;
    uint8_t data[0];
} fifoitem_t;

typedef struct _slist_ext {
    fifoitem_t *r_p, *w_p;	/* �J�����g�Q�Ƃ̃|�C���^ */
    uint8_t sizof_element;
    size_t cnt;
    fifoitem_t base;		/* �z��0�̍\���̂�����̂ŕK���Ō�ɂ��� */
} slist_ext_t;

#define get_slist_ext(s) (slist_ext_t*)((s)->ext)
#define get_const_slist_ext(s) (const slist_ext_t*)((s)->ext)

static fifoitem_t *slist_get_foward_element_ptr( const slist_ext_t *const e, void * const element_ptr);

/**
 * @fn static fifoitem_t *slist_get_foward_element_ptr( const stl_slist_ext_t *const e, void * const element_ptr)
 * @brief �w�肳�ꂽ�G�������g�o�b�t�@�̑O���̃G�������g�o�b�t�@�̃|�C���^��Ԃ��܂��B
 *	�O���֐���insert, erase����������ׂɎg�p���܂��B
 * @param stl_slist_ext_t�\���̃|�C���^
 * @param element_ptr ��Ƃ���G�������g�|�C���^
 * @retval NULL �O���̃|�C���^�͖���
 * @retval NULL�ȊO �O����fifo�G�������g�|�C���^
 */
static fifoitem_t *slist_get_foward_element_ptr( const slist_ext_t *const e, void * const element_ptr)
{
    static fifoitem_t *cur;

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
 * @fn int smal_slist_init( smal_slist_t *const self_p, const uint8_t sizof_element)
 * @brief �I�u�W�F�N�g�����������܂�
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @param sizof_element �G�������g�̃T�C�Y
 * @retval 0 ����
 * @retval EAGAIN ���\�[�X�̊l���Ɏ��s
 * @retval -1 ����ȊO�̒v���I�Ȏ��s
 */
int smal_slist_init( smal_slist_t *const self_p, const uint8_t sizof_element)
{
    static slist_ext_t *e;

    e = (slist_ext_t *)SMAL_MALLOC(sizeof(slist_ext_t));
    if (NULL == e) {
	return ERANGE;
    }

    self_p->ext = e;

    e->sizof_element = sizof_element;

    e->r_p = e->w_p = &e->base;
    e->cnt = 0;

    return 0;
}

/**
 * @fn int smal_slist_destroy( smal_slist_t *const self_p)
 * @brief �L���[�I�u�W�F�N�g��j�����܂�
 * @param self_p smal_slist_t�\���̃C���X�^���X�|�C���^
 * @retval -1 errno.h�ȊO�̒v���I�Ȏ��s
 * @retval 0 ����
 */
int smal_slist_destroy( smal_slist_t *const self_p)
{
    static slist_ext_t *e;
    /* initialize */
    e = get_slist_ext(self_p);

    if(smal_slist_clear(self_p)) {
	return -1;
    }

    /* �Ō�̃G�������g��base�Ŗ�����΍폜 */
    if (e->r_p != &e->base) {
	SMAL_FREE( e->r_p);
	e->r_p = NULL;
    }
    SMAL_FREE(self_p->ext);

    return 0;
}

/**
 * @fn int smal_slist_push(smal_slist_t *const self_p, const void *const el_p, const uint8_t sizof_element)
 * @brief �L���[�ɃG�������g��ǉ����܂�
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @param el_p �G�������g�|�C���^
 * @param sizof_element �G�������g�T�C�Y(mpsx_stl_slist_init�Ŏw�肵���ȊO�̃T�C�Y�̓G���[�Ƃ��܂�)
 * @retval 0 ����
 * @retval EINVAL �G�������g�T�C�Y���قȂ�
 * @retval EAGAIN ���\�[�X�̊l���Ɏ��s
 * @retval -1 ����ȊO�̒v���I�Ȏ��s
 */
int smal_slist_push(smal_slist_t *const self_p, const void *const el_p, const uint8_t sizof_element)
{
    static slist_ext_t *e;
    fifoitem_t *__restrict f;

    /* initialize */
    e = get_slist_ext(self_p);
    if (e->sizof_element != sizof_element) {
	return EINVAL;
    }

    f = (fifoitem_t *)SMAL_MALLOC( (sizeof(fifoitem_t) + e->sizof_element));
    if (NULL == f) {
	return EINVAL;
    }

    f->next = NULL;
    memcpy(f->data, el_p, e->sizof_element);

    /* �L���[�ɒǉ� */
    e->w_p->next = f;
    e->w_p = f;
    ++(e->cnt);

    return 0;
}

/**
 * @fn int smal_slist_pop(smal_slist_t *const self_p)
 * @brief �L���[�̐擪�̗v�f���폜���܂�
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @retval EINVAL �폜����G�������g�����݂��Ȃ�
 * @retval -1 ����ȊO�̒v���I�Ȏ��s
 * @retval 0 ����
 */
int smal_slist_pop(smal_slist_t *const self_p)
{
    static slist_ext_t *e;
    static fifoitem_t *__restrict tmp;

    /* initialize */
    e = get_slist_ext(self_p);
    /* �G�������g�����邩�ǂ�����d�`�F�b�N */
    if (e->cnt == 0 ) {
	return EINVAL;
    }

    if((e->r_p == &e->base) && ( e->r_p->next == NULL)) { // �����̌������悭������Ȃ��̂Œ��ׂ�K�v�L��
	return -1;
    }

    tmp = e->r_p;
    e->r_p = e->r_p->next;
    --(e->cnt);

    if (tmp != &e->base) {
	SMAL_FREE(tmp);
    }

    if (e->cnt == 0) {
	if (e->r_p != &e->base) {
	    SMAL_FREE(e->r_p);
	}
	e->r_p = e->w_p = &e->base;
    }

    return 0;
}

/**
 * @fn int smal_slist_front(smal_slist_t *const self_p, void *const el_p, const uint8_t sizof_element)
 * @brief �L���[�̐擪�̃G�������g�f�[�^��Ԃ��܂�
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @param el_p �G�������g�f�[�^�擾�p�f�[�^�o�b�t�@�|�C���^
 * @param sizof_element �G�������g�T�C�Y(mpsx_stl_slist_init�Ŏw�肵���ȊO�̃T�C�Y�̓G���[�Ƃ��܂�)
 * @retval EINVAL  �G�������g�T�C�Y���قȂ�
 * @retval EINVAL �G�������g�����݂��Ȃ�
 * @retval -1 ����ȊO�̒v���I�Ȏ��s
 * @retval 0 ����
 */
int smal_slist_front(smal_slist_t *const self_p, void *const el_p, const uint8_t sizof_element)
{
    static slist_ext_t *e;
    static fifoitem_t *__restrict f;

    /* initialize */
    e = get_slist_ext(self_p);
    if (e->sizof_element != sizof_element) {
	return EINVAL;
    } else if (e->cnt == 0) {
	/* �L���[�ɃG�������g�������ꍇ */
	return EINVAL;
    }
    f = e->r_p->next;

    memcpy(el_p, f->data, e->sizof_element);

    return 0;
}

/**
 * @fn int smal_slist_clear(smal_slist_t *const self_p)
 * @brief �L���[�ɒ��܂��Ă���G�������g�f�[�^��S�Ĕj�����܂�
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @retval -1 �v���I�Ȏ��s
 * @retval 0 ����
 */
int smal_slist_clear(smal_slist_t *const self_p)
{
    static slist_ext_t *e;
    static size_t n;

    /* initialize */
    e = get_slist_ext(self_p);
    if (e->cnt == 0) {
	return 0;
    }

    for (n = e->cnt; n != 0; --n) {
	if(smal_slist_pop(self_p)) {
	    return -1;
	}
    }

    return 0;
}

/**
 * @fn uint16_t smal_slist_get_pool_cnt(smal_slist_t *const self_p)
 * @brief �L���[�Ƀv�[������Ă���G�������g����Ԃ��܂�
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @retval 0�ȏ� �v�f��
 */
size_t smal_slist_get_pool_cnt(smal_slist_t *const self_p)
{
    static const slist_ext_t *e;
    /* initialize */
    e = get_const_slist_ext(self_p);

    return e->cnt;
}

/**
 * @fn int smal_slist_is_empty(smal_slist_t *const self_p)
 * @brief �L���[���󂩂ǂ����𔻒肵�܂�
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @retval -1 ���s
 * @retval 0 �L���[�͋�ł͂Ȃ�
 * @retval 1 �L���[�͋�ł���
 */
int smal_slist_is_empty(smal_slist_t *const self_p)
{
    static const slist_ext_t *e;
    /* initialize */
    e = get_const_slist_ext(self_p);

    return (e->cnt == 0) ? 1 : 0;
}

/**
 * @fn int mpsx_stl_slist_get_element_at( mpsx_stl_slist_t *const self_p, const size_t num, void *const el_p, const int sizof_element);
 * @brief �L���[�ɕۑ�����Ă���G�������g��Ԃ��܂�
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @param num 0��n�܂�L���[�擪����̃G�������g�z��ԍ�
 * @param el_p �G�������g�f�[�^�R�s�[�p�G�������g�\���̃|�C���^
 * @param sizof_element �G�������g�T�C�Y(��ɃG�������g�T�C�Y���،���)
 * @retval 0 ���� ���s
 * @retval EINVAL �L���[�ɃG�������g���Ȃ�
 * @retval EINVAL �w�肳�ꂽ�G�������g������
 * @retval EINVAL el_p�Ɏw�肳�ꂽ�A�h���X��NULL������
 * @retval EINVAL sizof_element�̃T�C�Y���قȂ�(�������j
 */
int smal_slist_get_element_at(smal_slist_t *const self_p,
				       const size_t num, void *const el_p,
				       const uint8_t sizof_element)
{
    static slist_ext_t *e;
    static fifoitem_t *__restrict item_p;
    size_t n;

    /* initialize */
    e = get_slist_ext(self_p);
    if ((NULL == el_p) ||(sizof_element < e->sizof_element)) {
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
 * @brief �L���[�̍Ō���̃G�������g�f�[�^��Ԃ��܂�
 * @param self_p smal_slist_t�\���̃C���X�^���X�|�C���^
 * @param el_p �G�������g�f�[�^�擾�p�f�[�^�o�b�t�@�|�C���^
 * @param sizof_element �G�������g�T�C�Y(mpsx_stl_slist_init�Ŏw�肵���ȊO�̃T�C�Y�̓G���[�Ƃ��܂�)
 * @retval EINVAL  �G�������g�T�C�Y���قȂ�
 * @retval EINVAL �G�������g�����݂��Ȃ�
 * @retval -1 ����ȊO�̒v���I�Ȏ��s
 * @retval 0 ����
 */
int smal_slist_back( smal_slist_t *const self_p, void *const el_p, const uint8_t  sizof_element )
{
    static slist_ext_t *e;
    fifoitem_t *__restrict tmp = NULL;

    /* initialize */
    e = get_slist_ext(self_p);
    /* �G�������g�����邩�ǂ�����d�`�F�b�N */
    if (e->cnt == 0) {
	return EINVAL;
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
 * @fn int smal_slist_insert_at( smal_slist_t *const self_p, const size_t no, void *const el_p, const uint8_t sizof_element)
 * @brief �w�肳�ꂽ�G�������g�ԍ��̑O�ɁA�G�������g�f�[�^��}�����܂��B
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @param no �I�u�W�F�N�g����0�`�̃��X�g�G�������g�ԍ�
 * @param el_p �G�������g�f�[�^�擾�p�f�[�^�o�b�t�@�|�C���^
 * @param sizof_element �G�������g�T�C�Y(mpsx_stl_slist_init�Ŏw�肵���ȊO�̃T�C�Y�̓G���[�Ƃ��܂�)
 * @retval EINVAL  �G�������g�T�C�Y���قȂ�
 * @retval ENOENT �w�肳�ꂽ�ԍ��̃G�������g�����݂��Ȃ�
 * @retval EAGAIN �������s���Œǉ��s�\
 * @retval -1 ����ȊO�̒v���I�Ȏ��s
 * @retval 0 ����
 */
int smal_slist_insert_at( smal_slist_t *const self_p, const size_t no, void *const el_p, const uint8_t sizof_element)
{
    static size_t n;
    static slist_ext_t *e;
    static fifoitem_t *__restrict cur_ticket_p;
    static fifoitem_t *__restrict fwd_ticket_p;
    static fifoitem_t *__restrict f;

    /* initialize */
    e = get_slist_ext(self_p);
    cur_ticket_p = &e->base;
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

    f = (fifoitem_t *) SMAL_MALLOC(sizeof(fifoitem_t) + e->sizof_element);
    if (NULL == f) {
	return -1;
    }

    memset(f, 0x0, sizeof(fifoitem_t));
    memcpy(f->data, el_p, e->sizof_element);
    f->next = NULL;

    /* �L���[�ɒǉ� */
    f->next = cur_ticket_p;
    fwd_ticket_p->next = f;
    ++(e->cnt);

    return 0;
}

/**
 * @fn int smal_slist_erase_at( smal_slist_t *const self_p, const size_t no)
 * @brief �w�肳�ꂽ�G�������g���������܂��B
 *      ����m�F��
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @param no �I�u�W�F�N�g����0�`�̃��X�g�G�������g�ԍ�
 * @retval 0 ����
 * @retval EINVAL �w�肳�ꂽ�ԍ��̃G�������g�����݂��Ȃ�
 **/
int smal_slist_erase_at( smal_slist_t *const self_p, const size_t no)
{
    static slist_ext_t *e;
    static fifoitem_t *__restrict tmp;
    static fifoitem_t *__restrict fwd_ticket_p;

    /* initialize */
    e = get_slist_ext(self_p);

    /* �G�������g�����邩�ǂ�����d�`�F�b�N */
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
	 SMAL_FREE(tmp);
    }

    if (e->cnt == 0) {
	if (e->r_p != &e->base) {
	    SMAL_FREE(e->r_p);
	}
	e->r_p = e->w_p = &e->base;
    }

    return 0;

}

/**
 * @fn int smal_slist_overwrite_element_at( smal_slist_t *const self_p, const size_t num, const void *const el_p, const size_t sizof_element)
 * @brief �w�肳�ꂽ�G�������g���㏑�����܂��B
 * @param self_p mpsx_stl_slist_t�\���̃C���X�^���X�|�C���^
 * @param no �I�u�W�F�N�g����0�`�̃��X�g�G�������g�ԍ�
 * @retval 0 ����
 * @retval EINVAL �w�肳�ꂽ�ԍ��̃G�������g�����݂��Ȃ�
 **/
int smal_slist_overwrite_element_at( smal_slist_t *const self_p, const size_t num, const void *const el_p, const size_t sizof_element)
{
    static slist_ext_t *e;
    static fifoitem_t *__restrict item_p;
    size_t n;

    /* initialize */
    e = get_slist_ext(self_p);
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

    memcpy( item_p->next->data, el_p, e->sizof_element);


    return 0;


}
