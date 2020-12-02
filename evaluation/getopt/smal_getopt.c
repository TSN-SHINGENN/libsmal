/**
 *      Copyright 2020 TSN�SHINGENN All Rights Reserved.
 *
 *      Basic Author: Seiichi Takeda  '2020-July-02 Active
 *              Last Alteration $Author: takeda $
 */

/**
 * @file smal_getopt.c
 * @brief POSIX getopt()�̃t���X�N���b�`��
 *	�����y�ђP��ɂ��R�}���h���C���I�v�V������͊֐� getopt()�G�~�����[�V�����֐�
 *	POSIX��getopt()�Ɋ����Ȃ����ɂȂ��Ă��܂��B
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


/* this */
#include "smal_getopt.h"

#define BADCHAR ('?')
#define BADARG (':')
#define RETFINI (-1)
#define RETHYPHEN (-2)

#if 0
static const char *const null_msg = "";
#endif

typedef enum _enum_getopt_type {
    getopt_type_is_long,
    getopt_type_is_long_only
} enum_getopt_type_t;


static int own_getopt_exec(smal_getopt_t * const __restrict self_p,
			   const int argc, const char *const argv[],
			   const char *const ostr);
static int own_getopt_long_exec(smal_getopt_t * const __restrict self_p,
				const enum_getopt_type_t getopt_type,
				const int argc, const char *const argv[],
				const char *const optstring,
				const smal_getopt_long_option_t * const lopts,
				int *const index_p);

/**
 * @fn smal_getopt_t smal_getopt_initializer(void)
 * @brief smal_getopt_t�\���̃C���X�^���X������������ׂɏ����l�������܂��B
 * @return smal_getopt_t�\���̏����l
 */
smal_getopt_t smal_getopt_initializer(void)
{
    smal_getopt_t ret;

    ret.argc = -1;
    ret.optopt = 0;
    ret.optind = 1;
    ret.optarg = NULL;
    ret._sp = 1;

    return ret;
}

/**
 * @fn static int own_getopt_exec(smal_getopt_t * const self_p, int argc, char *argv[], const char *const ostr)
 * @brief �R�}���h���C��������͂��s���i�����ł̎��́j
 * @param self_p smal_getopt_t�\���̃C���X�^���X�|�C����
 * @param argc ��������
 * @param argv �����z��|�C���^
 *	�� argv �̗v�f�̂��� '-' �Ŏn�܂���̂̓I�v�V�����v�f (option element) �Ƃ݂Ȃ����B
 *	    ���̗v�f����擪�� '-' �y�уI�v�V�����v�f������������������ �I�v�V�������� (option character) �Ƃ����B
 *	    �܂��A�I�v�V�����v�f�͋󔒕����������Ď��̈����悤���Ƃ��邱�Ƃ��ł���B
 * @param optstring �󂯕t�����I�v�V������������Ȃ镶���� Linux����getopt()���Q�Ƃ��Ă��������B
 *	�� NULL�w��̏ꍇ�� '--'�̌��o�݂̂����Ȃ�
 * @retval 0�ȏ� �I�v�V���������R�[�h
 * @retval -1(RETFINI) ��͂̏I��������
 * @retval -2(RETHYPHEN) '--'�����o���ꂽ(ostr��NULL���w�肳�ꂽ�ꍇ��'-'�����l)
 */
static int own_getopt_exec(smal_getopt_t * const __restrict self_p,
			   const int argc, const char *const argv[],
			   const char *const ostr)
{
    /**
     * This code is public document which was powered by AT&T
     * custmized by Seiichi.Takeda Vw Dep.
     */

    int c;
    const char *cp;

#if 0
    IFDBG3THEN {
	const size_t xtoa_bufsz=smal_XTOA_DEC64_BUFSZ;
	char xtoa_buf[smal_XTOA_DEC64_BUFSZ];

	    smal_i64toadec( (int64_t)argc, xtoa_buf, xtoa_bufsz);
	DBMS3(stderr, "own_getopt_exec : argc=%s ostr=%s" EOL_CRLF,
	    xtoa_buf, ostr);
    }
#endif

    if ((self_p->argc < 0) || (self_p->_sp == 1)) {
	if (self_p->optind >= argc || (argv[self_p->optind][0] != '+' &&
				       argv[self_p->optind][0] != '-')
	    || argv[self_p->optind][1] == '\0') {
	    return RETFINI;
	} else if (strcmp(argv[self_p->optind], "--") == 0) {
//	    DBMS3(stderr, "found --" EOL_CRLF);
	    ++(self_p->optind);
	    return RETHYPHEN;
	}
#if 0
	/* '+' for config options, '+' should not be in the opts list */
	if (argv[optind][0] == '+') {
	    optarg = argv[optind++] + 1;
	    return '+';
	}
#endif
    }

    if (NULL == ostr) {
	self_p->_sp = 1;
	return RETHYPHEN;
    }

    self_p->optopt = c = argv[self_p->optind][self_p->_sp];
    if (c == ':' || (cp = strchr(ostr, c)) == NULL) {
#if 0
	DBMS5(stderr, "own_getopt_exec : illegal option -- %c" EOL_CRLF,
	      self_p->optopt);
#endif
	if (argv[self_p->optind][++self_p->_sp] == '\0') {
	    ++(self_p->optind);
	    self_p->_sp = 1;
	}
	return BADCHAR;
    }

    if (*++cp == ':') {
	if (argv[self_p->optind][(self_p->_sp) + 1] != '\0') {
	    self_p->optarg = &argv[(self_p->optind)++][(self_p->_sp) + 1];
	} else if (++(self_p->optind) >= argc) {
#if 0
	    DBMS1(stdout,
		  "own_getopt_exec : '%c' option requires an argument -- " EOL_CRLF,
		  c);
#endif
	    self_p->_sp = 1;
	    return BADARG;
	} else {
	    self_p->optarg = argv[(self_p->optind)++];
	}

	self_p->_sp = 1;
    } else {
	if (argv[self_p->optind][++(self_p->_sp)] == '\0') {
	    self_p->_sp = 1;
	    self_p->optind++;
	}
	self_p->optarg = NULL;
    }

    return c;
}


/**
 * @fn int smal_getopt(smal_getopt_t * self_p, int argc, char *argv[], const char *optstring)
 * @brief �R�}���h���C��������͂��s��
 * @param self_p smal_getopt_t�\���̃C���X�^���X�|�C����
 * @param argc ��������
 * @param argv �����z��|�C���^
 *	�� argv �̗v�f�̂��� '-' �Ŏn�܂���̂̓I�v�V�����v�f (option element) �Ƃ݂Ȃ����B
 *	    ���̗v�f����擪�� '-' �y�уI�v�V�����v�f������������������ �I�v�V�������� (option character) �Ƃ����B
 *	    �܂��A�I�v�V�����v�f�͋󔒕����������Ď��̈����悤���Ƃ��邱�Ƃ��ł���B
 * @param optstring �󂯕t�����I�v�V������������Ȃ镶���� Linux����getopt()���Q�Ƃ��Ă��������B
 * @retval 0�ȏ� �I�v�V���������R�[�h
 * @retval -1(RETFINI) ��͂̏I��������
 */
int smal_getopt(smal_getopt_t *const self_p, const int argc, const char *const argv[],
		   const char *optstring)
{
    int retval;

    if ((retval =
	 own_getopt_exec(self_p, argc, argv, optstring)) == RETHYPHEN) {
	retval = RETFINI;
	++(self_p->optind);
    }
    return retval;
}


/**
 * @fn static int own_getopt_long_exec(smal_getopt_t * const self_p, const enum_getopt_long_type_t long_type, int argc, const char * const argv[], const char * const optstring, smal_getopt_long_option_t * const lopts, int *index_p);
 * @brief �P��Ή��̃R�}���h������͂������Ȃ��i���������֐�)
 * @param self_p smal_getopt_t�\���̃C���X�^���X�|�C���^
 * @param is_only own_getopt_exec()���̌ĂԂ���ɌĂԂ������肷��( 0:�� 0�ȊO:��j
 * @param argc ��������
 * @param argv �����z��|�C���^
 *	�� argv �̗v�f�̂��� '-' �Ŏn�܂���� (���� "-" �P�Ƃ� "--" �P�Ƃł͂Ȃ�����) �� �I�v�V�����v�f (option element) �Ƃ݂Ȃ����B
 *	  ���̗v�f����擪�� '-' �������������� �I�v�V�������� (option character) �Ƃ����B
 * @param optstring �󂯕t����I�v�V������������Ȃ镶����
 *	�� �w�薳���̏ꍇ NULL �ł͖��� ""���w��
 *	�� �����̂��ƂɃR���� (:) ���u����Ă���ꍇ�́A�I�v�V�����ɂ͈��������K�v�ł��邱�Ƃ������B
 *	  ���̂Ƃ� getopt() �́A���ݒ��ڂ��Ă��� argv �v�f�ŁA�I�v�V���������Ɉ��������e�L�X�g�ւ̃|�C���^���A���邢�͎��� argv �v�f�̃e�L�X�g�ւ̃|�C���^�� optarg �ɑ������B 
 *	  2 �A�����ăR����(:)���u����Ă���ꍇ�́A ���̃I�v�V�����͈��������Ƃ��Ă��Ƃ�Ȃ��Ă��悢�B
 *	  ?�͗\�񂳂�Ă���̂Ŏg���Ȃ�
 * @param longopts �P��^�L�[���[�h�z��|�C���^
 * @param longindex NULL�ȊO��longopts�Ŏw�肳�ꂽ�����I�v�V�����̃C���f�b�N�X�� longopts ����̑��Έʒu�Ƃ��ĕԂ��ϐ��|�C���^
 * @retval 1�ȏ� optstring�̃L�[���[�h���������������炻�̕����ԍ�
 * @retval 0 ���o���ꂽ�L�[���[�h������̃p�����[�^��flag��NULL�ȊO�̏ꍇ��val�l��Ԃ�
 * @retval -1(RETFINI) ��͂̏I��������
 */
static int own_getopt_long_exec(smal_getopt_t * const __restrict self_p,
				const enum_getopt_type_t getopt_type,
				const int argc, const char *const argv[],
				const char *const optstring,
				const smal_getopt_long_option_t * const lopts,
				int *const index_p)
{
    int retval;

    if (!(self_p->optind < argc)) {
	self_p->_sp = 1;
	return RETFINI;
    }

    if (getopt_type == getopt_type_is_long) {
	retval = own_getopt_exec(self_p, argc, argv, optstring);
    } else {
	if (*argv[self_p->optind] != '-') {
	    return RETFINI;
	}
	retval = -1;
    }
    if ((retval == RETHYPHEN) || (getopt_type == getopt_type_is_long_only)) {
	const char *cur_argv = argv[self_p->optind], *has_equal;
	int match;
	size_t i;
	size_t cur_argv_len;

//	DBMS3(stderr, "getopt_long_exec : cur_argv = %s" EOL_CRLF, cur_argv);

	for (i = 0; i < 2; ++i) {
	    if (*cur_argv == '-') {
		++cur_argv;
	    } else {
		break;
	    }
	}

//	DBMS3(stderr, "cur_argv = %s" EOL_CRLF, cur_argv);
	if (*cur_argv == '\0') {
	    return RETFINI;
	}
	if ((has_equal = strchr(cur_argv, '=')) != NULL) {
	    cur_argv_len =
		(size_t) ((uintptr_t) has_equal - (uintptr_t) cur_argv);
	    ++has_equal;
	} else {
	    cur_argv_len = strlen(cur_argv);
	}

	if ((NULL != lopts) && (NULL != lopts[0].name)) {
	    for (match = 0; NULL != lopts[match].name; ++match) {
		if (strncmp(cur_argv, lopts[match].name, cur_argv_len)) {
		    continue;
		}
		if (strlen(lopts[match].name) == cur_argv_len) {
		    break;
		}
	    }
	    if (NULL == lopts[match].name) {
		match = -1;
	    }
	} else {
	    match = -1;
	}
	if (match != -1) {
	    const smal_getopt_long_option_t *t = &lopts[match];
	    ++(self_p->optind);
	    switch (t->has_arg) {
	    case SMAL_GETOPT_NO_ARGUMENT:
		self_p->optarg = NULL;
		break;
	    case SMAL_GETOPT_OPTIONAL_ARGUMENT:
	    case SMAL_GETOPT_REQUIRED_ARGUMENT:
		if (has_equal) {
		    self_p->optarg = has_equal;
		} else {
		    self_p->optarg = argv[self_p->optind];
		    ++(self_p->optind);
		}

		if (t->has_arg == SMAL_GETOPT_REQUIRED_ARGUMENT) {
		    if (self_p->optarg == NULL) {
			if (*optstring != ':') {
#if 0
			DBMS5(stderr,
				  "option requires an argument -- %s" EOL_CRLF,
				  cur_argv);
#endif
			    return BADARG;
			}
		    }
		}
		break;
	    case SMAL_GETOPT_eot:
		return BADARG;
	    }
	} else {
	    /* was not matched */
	    if (getopt_type == getopt_type_is_long_only) {
		if ((retval =
		     own_getopt_exec(self_p, argc, argv,
				     optstring)) == RETHYPHEN) {
		    retval = RETFINI;
		}
		++(self_p->optind);
		return retval;
	    } else {
		if (*optstring != ':') {
//		    DBMS5(stderr, "illegal option -- %s" EOL_CRLF, cur_argv);
		}
		++(self_p->optind);
		return BADCHAR;
	    }
	}
	if (NULL != lopts[match].flag_p) {
	    *lopts[match].flag_p = lopts[match].val;
	    retval = 0;
	} else {
	    retval = lopts[match].val;
	}
	if (NULL != index_p) {
	    *index_p = match;
	}
    }
    return retval;
}

/**
 * @fn int smal_getopt_long(smal_getopt_t *self_p, int argc, char *argv[], const char *optstring, smal_getopt_long_option_t *lopts, int *index_p)
 * @brief �P��Ή��̃R�}���h������͂������Ȃ�
 * @param self_p smal_getopt_t�\���̃C���X�^���X�|�C���^
 * @param argc ��������
 * @param argv �����z��|�C���^
 *	�� argv �̗v�f�̂��� '-' �Ŏn�܂���� (���� "-" �P�Ƃ� "--" �P�Ƃł͂Ȃ�����) �� �I�v�V�����v�f (option element) �Ƃ݂Ȃ����B
 *	    ���̗v�f����擪�� '-' �������������� �I�v�V�������� (option character) �Ƃ����B
 * @param optstring �󂯕t����I�v�V������������Ȃ镶����
 *	�� �w�薳���̏ꍇ NULL �ł͖��� ""���w��
 *	�� �����̂��ƂɃR���� (:) ���u����Ă���ꍇ�́A�I�v�V�����ɂ͈��������K�v�ł��邱�Ƃ������B
 *	  ���̂Ƃ� getopt() �́A���ݒ��ڂ��Ă��� argv �v�f�ŁA�I�v�V���������Ɉ��������e�L�X�g�ւ̃|�C���^���A���邢�͎��� argv �v�f�̃e�L�X�g�ւ̃|�C���^�� optarg �ɑ������B 
 *	  2 �A�����ăR����(:)���u����Ă���ꍇ�́A ���̃I�v�V�����͈��������Ƃ��Ă��Ƃ�Ȃ��Ă��悢�B
 *	  ?�͗\�񂳂�Ă���̂Ŏg���Ȃ�
 * @param lopts �P��^�L�[���[�h�z��|�C���^
 * @param index_p NULL�ȊO��longopts�Ŏw�肳�ꂽ�����I�v�V�����̃C���f�b�N�X�� longopts ����̑��Έʒu�Ƃ��ĕԂ��ϐ��|�C���^
 * @retval 1�ȏ� optstring�̃L�[���[�h���������������炻�̕����ԍ�
 * @retval 0 ���o���ꂽ�L�[���[�h������̃p�����[�^��flag��NULL�ȊO�̏ꍇ��val�l��Ԃ�
 * @retval -1 ��͂̏I��������
 */
int smal_getopt_long(smal_getopt_t *const self_p, const int argc, const char *const argv[],
			const char *const optstring,
			const smal_getopt_long_option_t *const lopts, int *const index_p)
{
    return own_getopt_long_exec(self_p, getopt_type_is_long, argc, argv,
				optstring, lopts, index_p);
}

/**
 * @fn int smal_getopt_long_only(smal_getopt_t *self_p, int argc, char *argv[], const char *optstring, smal_getopt_long_option_t *lopts, int *index_p)
 * @brief �P��Ή��̃R�}���h������͂������Ȃ�
 *	��{�I��smal_getopt_long()�Ɠ����Ȃ̂ŁA �ڍׂ�smal_getopt_long���Q�ƁB
 *	�قȂ�_�� '--'��'-'�������Ɉ����邱�ƁB�����I�v�V�����͐�ɕ]������A���ɒZ���I�v�V������]������.
 * @param self_p smal_getopt_t�\���̃C���X�^���X�|�C���^
 * @param argc ��������
 * @param argv �����z��|�C���^
 *	�� argv �̗v�f�̂��� '-' �Ŏn�܂���� (���� "-" �P�Ƃ� "--" �P�Ƃł͂Ȃ�����) �� �I�v�V�����v�f (option element) �Ƃ݂Ȃ����B
 *	   ���̗v�f����擪�� '-' �������������� �I�v�V�������� (option character) �Ƃ����B
 * @param optstring �󂯕t����I�v�V������������Ȃ镶����
 *	�� �w�薳���̏ꍇ NULL �ł͖��� ""���w��
 *	�� �����̂��ƂɃR���� (:) ���u����Ă���ꍇ�́A�I�v�V�����ɂ͈��������K�v�ł��邱�Ƃ������B
 *	  ���̂Ƃ� getopt() �́A���ݒ��ڂ��Ă��� argv �v�f�ŁA�I�v�V���������Ɉ��������e�L�X�g�ւ̃|�C���^���A���邢�͎��� argv �v�f�̃e�L�X�g�ւ̃|�C���^�� optarg �ɑ������B 
 *	  2 �A�����ăR����(:)���u����Ă���ꍇ�́A ���̃I�v�V�����͈��������Ƃ��Ă��Ƃ�Ȃ��Ă��悢�B
 *	  ?�͗\�񂳂�Ă���̂Ŏg���Ȃ�
 * @param lopts �P��^�L�[���[�h�z��|�C���^
 * @param index_p NULL�ȊO��longopts�Ŏw�肳�ꂽ�����I�v�V�����̃C���f�b�N�X�� longopts ����̑��Έʒu�Ƃ��ĕԂ��ϐ��|�C���^
 * @retval 1�ȏ� optstring�̃L�[���[�h���������������炻�̕����ԍ�
 * @retval 0 ���o���ꂽ�L�[���[�h������̃p�����[�^��flag��NULL�ȊO�̏ꍇ��val�l��Ԃ�
 * @retval -1 ��͂̏I��������
 */
int smal_getopt_long_only(smal_getopt_t *const self_p, const int argc,
			     const char *const argv[], const char *const optstring,
			     const smal_getopt_long_option_t *const lopts,
			     int *const index_p)
{
    return own_getopt_long_exec(self_p, getopt_type_is_long_only, argc,
				argv, optstring, lopts, index_p);
}
