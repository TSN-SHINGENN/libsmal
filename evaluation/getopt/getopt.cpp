// getopt.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <stdio.h>
#include "smal_getopt.h"

/* Japan Linux の JM getopt サンプルコードから */

static int
getopt_long_test(smal_getopt_t *const s, int argc, char **argv) {
    int c;
    int digit_optind = 0;

    /* optarg と optindは オブジェクト内のを使う  */
    /* struct option は　smal_getopt_long_option_tにする */

    while (1) {
        int this_option_optind = s->optind ? s->optind : 1;
        int option_index = 0;

        static smal_getopt_long_option_t long_options[] = {
            { "add",     SMAL_GETOPT_REQUIRED_ARGUMENT, 0,  0 }, // a
            { "append",  SMAL_GETOPT_NO_ARGUMENT,       0,  0 }, // b
            { "delete",  SMAL_GETOPT_REQUIRED_ARGUMENT, 0,  0 }, // c
            { "verbose", SMAL_GETOPT_NO_ARGUMENT,       0,  0 }, // d
            { "create",  SMAL_GETOPT_REQUIRED_ARGUMENT, 0, 'c'}, // c だけど ロングができない?? 
            { "file",    SMAL_GETOPT_REQUIRED_ARGUMENT, 0,  0 },
            { 0,         SMAL_GETOPT_eot,               0,  0 }
        };

	/* オプションの並びと構造体の配列が一緒になる */

	c = smal_getopt_long( s, argc, argv, "abc:d:012",
                 long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            printf("option %s", long_options[option_index].name);
            if (s->optarg)
                printf(" with arg %s", s->optarg);
            printf("\n");
            break;

        case '0':
        case '1':
        case '2':
            if (digit_optind != 0 && digit_optind != this_option_optind)
              printf("digits occur in two different argv-elements.\n");
            digit_optind = this_option_optind;
            printf("option %c\n", c);
            break;

        case 'a':
            printf("option a\n");
            break;

        case 'b':
            printf("option b\n");
            break;

        case 'c':
            printf("option c with value '%s'\n", s->optarg);
            break;

        case 'd':
            printf("option d with value '%s'\n", s->optarg);
            break;

        case '?':
            break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (s->optind < argc) {
        printf("non-option ARGV-elements: ");
        while (s->optind < argc)
            printf("%s ", argv[s->optind++]);
        printf("\n");
    }

    // exit(EXIT_SUCCESS);
    return 0;
}

static int
getopt_test( smal_getopt_t *const s, int argc, char **argv)
{
    int flags, opt;
    int nsecs, tfnd;
    /* optarg と optindは オブジェクト内のを使う  */

    nsecs = 0;
    tfnd = 0;
    flags = 0;
    while ((opt = smal_getopt( s, argc, argv, "nt:")) != -1) {
        switch (opt) {
        case 'n':
            flags = 1;
            break;
        case 't':
	    nsecs = atoi(s->optarg);
            tfnd = 1;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
                    argv[0]);
            // exit(EXIT_FAILURE);
	    return 1;
        }
    }

    printf("flags=%d; tfnd=%d; nsecs=%d; optind=%d\n",
            flags, tfnd, nsecs, s->optind);

    if (s->optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        // exit(EXIT_FAILURE);
	return 1;
    }

    printf("name argument = %s\n", argv[s->optind]);

    /* Other code omitted */

    // exit(EXIT_SUCCESS);
    return 0;
}


int main( int argc, char **argv)
{
    smal_getopt_t go = smal_getopt_initializer();
    smal_getopt_t glo = smal_getopt_initializer();

    std::cout << "Hello World!\n";

    fprintf( stdout, "argc=%d\n", argc);
    
    fprintf( stdout, "\n");
    fprintf( stdout, "getopt_test\n");
    getopt_test( &go, argc, argv);

    fprintf( stdout, "\n");
    fprintf( stdout, "getopt_long_test\n");
    getopt_long_test( &glo, argc, argv);

    return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
