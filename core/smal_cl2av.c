/**
 *      Copyright 2020 TSN�SHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-July-02 Active
 *              Last Alteration $Author: takeda $
 */

/**
 * @file smal_cl2av.cpp
 *   �R�}���h���C�������������C�u����
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
 * @brief own_strtok�I�u�W�F�N�g�����������܂�
 * @return ���������ꂽown_strtok_t�C���X�^���X
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
 * @brief �f���~�^��F������NULL�ŋ�؂肻�̃g�[�N���Ƃ��܂��B���̃g�[�N���̃|�C���^��Ԃ��܂��B
 *	delim2�ň͂܂ꂽ�����͂������̃g�[�N���Ƃ��ċ�؂�܂�
 * @param self_p own_strtok�C���X�^���X�|�C���^
 * @param result_pointer ���̃g�[�N���ւ̃|�C���^�܂��A ���̃g�[�N�����Ȃ���� NULL ��Ԃ��B�|�C���^
 * @param s ��͑Ώۂ̕�����o�b�t�@�|�C���^(��ڈȍ~�̃g�[�N���̉�͂ɂ�NULL���w�肵�܂��j
 *	�w�肳�ꂽ�o�b�t�@�[�̓g�[�N���ɕ������邽�߃f�[�^��ύX���܂�
 * @param delim �f���~�^��ASCII�����W����
 * @param delim2 �͂�����ASCII����('\0'�Ŗ���)
 * @retval 0 ����
 * @retval EINVAL �������s��
 * @retval EIO �͂������̏I�[��������Ȃ�
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
	    /* �͂������ƃf���~�^���d�Ȃ��Ă��� */
  	    return EINVAL;
	}
    } 

    /* �͂��������ݒ肳��Ă��āA�擪����͂��������o�Ă����ꍇ�͐���̋�؂蕶����΂��͍s��Ȃ� */
    if(('\0' != *self_p->strtok_pbuf) && (delim2 != *self_p->strtok_pbuf)) {
	char * __restrict p = self_p->strtok_pbuf;
	int a, b, c;

	/* �擪����I�[�����E�͂����� �ł͂Ȃ��A��؂蕶���̏ꍇ�̓C���N�������g���Ĕ�΂� */
	while(((a=('\0' != *p)) && (b=( delim2 != *p ))) && (c=(NULL != strchr(delim, *p)))) {
	    /* �󔒕����̈���������Ȃ̂Ō��o���͒E�o���� */
	    if (!(self_p->attr.f.untrim_space) && (' ' == *p) ) {
		break;
	    }
	    ++p;
	}
	self_p->strtok_pbuf = p;

	/* ���łɏI�[�Ȃ甲���� */
	if( '\0' == *(self_p->strtok_pbuf) ) {
	    *result_pointer = NULL;
	    status = 0;
	    goto out;
	}
    }

    /* �擪�̃f���~�^���΂�����c��̕������𐔂��� */
    remain_length = strlen(self_p->strtok_pbuf);
    if (remain_length == 0) {
	*result_pointer = NULL;
	status = 0;
	goto out;
    }

    /* �t���O�������Ă��ꍇ�X�y�[�X�����O���� */
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

    /* ���̃f���~�^�̈ʒu(�g�[�N���̏I���)��T�� */
    delim_point = strpbrk(self_p->strtok_pbuf, delim);
    if ('\0' != delim2) {
	delim2_point = strchr(self_p->strtok_pbuf, delim2);
    }
    if ((NULL == delim_point) && (NULL == delim2_point)) {
	*result_pointer = self_p->strtok_pbuf;
	self_p->strtok_pbuf += (remain_length - n);	/* Trim������������ */
	status = 0;
	goto out;
    } else if ((delim_point == NULL)
	       || (delim_point == self_p->strtok_pbuf)
	       || ('\0' == *(self_p->strtok_pbuf))) {
	/* ����ȏ�A�g�[�N���͖��� */
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
	/* �͂������ň͂܂�ĂȂ������ꍇ */
	*delim_point = '\0';
	*result_pointer = self_p->strtok_pbuf;
	self_p->strtok_pbuf = delim_point + 1;
	break;

    case IN_FENCE:
	/* �͂������ň͂܂ꂽ�ꍇ */

	/* 1.�͂������̐擪���΂� */
	self_p->strtok_pbuf = delim2_point + 1;

	/* 2.�͂������̏I����T�� */
	delim2_point = strchr(self_p->strtok_pbuf, delim2);

	/* 3a. ������Ȃ��ꍇ */
	if (NULL == delim2_point) {
	    /* �G���[���^�[�� */
	    status = EIO;
	    goto out;
	}

	/* 3b. ���������ꍇ�͂����������� */

	*result_pointer = self_p->strtok_pbuf;
	*delim2_point = '\0';
	self_p->strtok_pbuf = delim2_point + 1;

	/* ����Ƀg�[�N���������ꍇ��1�����g�[�N�����΂� */
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
 * @brief ��͏������̌���Ƀg�[�N���ŕ�������K�v�����邩�ǂ������m�F���܂��B
 * @param self_p own_strtok�C���X�^���X�|�C���^
 * @param �g�[�N���������邽�߂̕�����
 * @retval 0 �������͂���K�v�L��
 * @retval 0�ȊO ����͂��łɉ�͂���K�v�Ȃ�(�I�[�ɒB���Ă���)
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
 * @brief ���������͂��ėv�f�ɕ������܂��B�I�u�W�F�N�g�̂���Ȃ�lite�łł�
 *	������str�̕�����͉�͎��ɏ����������܂��B
 * @param str �v�f���Ƃɕ������镶����o�b�t�@�ł��B������Ƀo�b�t�@���e�����������܂�
 * @param delim �f���~�^��ASCII�����W����( NULL �� " "�Ɠ��� )
 * @param delim2 �͂�����ASCII����('\0'�Ŗ���)
 * @param argc �v�f����Ԃ����߂̕ϐ��|�C���^
 * @param argv �v�f�ɕ������ꂽ������̃|�C���^�W����Ԃ��܂��B�z�񐔂͎��̈���limit���m�ۂ��Ă�������
 * @param limit �v�f�����̍ő吔��Ԃ��܂��B argv�z��𒴂��āA�o�b�t�@�[�I�[�o�������N�����Ȃ����߂̃��~�b�^�ł��B
 * @retval 0 ����
 * @retval EINVAL �������s��
 * @retval ERANGE limit�𒴂��Ĉ�����͂����悤�Ƃ���
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

