// gamma.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#if 1  /* def WIN32 */
#if _MSC_VER >= 1400 /* VC++2005 */
#pragma warning ( disable:4996 )
#pragma warning ( disable:4819 )
#endif
#endif

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "smal_gamma.h"

static float gamma = 0.8; // 適宜
uint8_t LUT[256];

#if 0
// 補正テーブルの作成
int smal_gamma8_init(uint8_t *const tab, const double g, const uint8_t offset, const uint16_t full_scale)
{
    static uint16_t n;
    const int16_t count = full_scale + 1;

    if(full_scale > 0xff) {
	return -1;
    }
    for (n = 0; n < count; n++){
	tab[n] = (int)(((double)full_scale * pow((double)n / (double)full_scale, 1.0 / g)) + offset);
    }

    return 0;
}
#endif


int main()
{
    FILE *fp = 0;
    int n;
    // smal_gamma8_init(LUT, gamma);
    const uint16_t l1 = 50;
    const uint16_t l2 = 255;
    
    smal_gamma8_init(LUT, 0.4, 0, l1);
    smal_gamma8_init(&LUT[l1], 1.5, LUT[l1-1]+1, l2-l1);


//    smal_gamma8_init(LUT, 0.8, 0, 255);


    fp = fopen("gamma.csv", "w");
    if(NULL == fp) {
	fprintf( stdout, "ファイルをオーブンできませんでした。\n");
	return 1;
    }
    for( n=0; n<256; ++n) {
	fprintf( stdout, "%d , %d\n", n, LUT[n]);
	fprintf( fp, "%d , %d\n", n, LUT[n]);
    }

    fclose(fp);

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
