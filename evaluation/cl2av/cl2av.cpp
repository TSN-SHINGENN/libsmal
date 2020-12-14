// cl2av.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#if 1 || defined(WIN32)
#pragma warning(disable:4996)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <iostream>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "smal_cl2av.h"

int main()
{
#define CMDBUFSZ 256
    char cmdbuf[CMDBUFSZ];
    int result;
    int ac;
    char *av[16];

    while (1) {
	printf( ">");
	fgets( cmdbuf, CMDBUFSZ, stdin);
	printf("cmdbuf=%s\n", cmdbuf);

	result = smal_cl2av_lite( cmdbuf, " \t\r\n", '\"', &ac, av, 16, NULL);

	if(result) {
	    printf("Syntax error, result=%d, strerror=%s\n", result, strerror(result));
	} else {
	    size_t n;
	    printf("ac = %d\n", ac);
	    for(n=0; n<ac; ++n) {
		printf( "av[%d]=%s\n", (int)n, av[n]);
	    }
	}
    }
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
