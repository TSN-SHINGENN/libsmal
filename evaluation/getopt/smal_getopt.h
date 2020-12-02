#ifndef INC_SMAL_GETOPT_H
#define INC_SMAL_GETOPT_H

#pragma once

/**
 * @var enum_smal_getopt_long_has_arg_t
 * @brief �I�v�V�����̈�����\���v�f
 */
typedef enum _enum_smal_getopt_long_has_arg {
    SMAL_GETOPT_NO_ARGUMENT = 0,			/*!< �I�v�V�����͈������Ƃ�Ȃ� */
    SMAL_GETOPT_REQUIRED_ARGUMENT = 1,		/*!< �I�v�V�����͈�����K�v�Ƃ��� */
    SMAL_GETOPT_OPTIONAL_ARGUMENT = 2,		/*!< �I�v�V�����͈������Ƃ��Ă��Ƃ�Ȃ��Ă��ǂ� */
    SMAL_GETOPT_eot = -1				/*!< �I�v�V�����z��I�[�p */
} enum_smal_getopt_long_has_arg_t;

/**
 * @var smal_getopt_long_option_t
 * @brief smal_getopt_long()�ɓn���I�v�V�����L�[
 * 	�� �z��̍Ō�̗v�f�́A�S�� NULL�܂���0 �Ŗ��߂�
 */
typedef struct _smal_getopt_long_option {
    const char *name;				/*!< �����I�v�V�����̖��O */
    enum_smal_getopt_long_has_arg_t has_arg; /*!< �I�v�V�����̎��� */
    int *flag_p;				/*!< NULL�̏ꍇval�l��Ԃ��B
    						     NULL�ȊO�͊֐���0��Ԃ�,���̃|�C���^�̕ϐ���val�l������� */
    int val;					/*!< flag �����߂��|�C���^�ɑ������l */
    const char *memo;				/*!< �g�����Ƃ�����(�֐������ɓ��ɈӖ��͂Ȃ�) */
} smal_getopt_long_option_t;

/**
 * @var smal_getopt_t
 * @brief smal_getopt????�𗘗p����ׂ̃I�u�W�F�N�g�\����
 */
typedef struct _smal_getopt {
    /* public */
    int argc;		/*!< �����̐� */
    int optind;		/*!< ���ɏ��������v�f�̃C���f�b�N�X */
    int optopt;		/*!< �G���[�ɂȂ����I�v�V���������R�[�h */
    const char *optarg; /*!< �I�v�V�����Ɉ���������ꍇ�̕�����̐擪�|�C���^(�Ȃ��ꍇ��NULL) */

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
