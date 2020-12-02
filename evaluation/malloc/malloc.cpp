// malloc.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#ifdef WIN32
#if _MSC_VER >= 1400 /* VC++2005 */
#pragma warning ( disable:4996 )
#pragma warning ( disable:4819 )
#endif
#endif
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include "smal_mallocater.h"
#include "smal_slist.h"

#define SLHEAPSZ 4096
uint8_t slist_heap[SLHEAPSZ];

static int test_slist(void)
{
    smal_slist_t sl;
    int status = -1, result;
    unsigned int n;
    int a;

    fprintf( stderr, " test : default\n");
    fprintf( stderr, "instance init\n");

    result = smal_slist_init( &sl, (uint8_t)sizeof(int), slist_heap, SLHEAPSZ);
    if(result) {
	fprintf( stderr, "smal_slist_init fail, strerror=%s\n", strerror(result));
	goto out;
    }
    result = smal_slist_is_empty( &sl );
    fprintf( stderr, "is_empty : %d\n", result);

    fprintf( stderr, "push\n");
    for( n='0'; n<='z'; ++n) { 
	result = smal_slist_push( &sl, &n, (uint8_t)sizeof(n));
	if(result) {
	    fprintf( stderr, "smal_slist_push fail, strerror=%s\n", strerror(result));
	    goto out;
	}
    }
    fprintf( stderr, "poolcnt = %llu\n", (long long)smal_slist_get_pool_cnt(&sl));

    fprintf( stderr, "get element\n");
    for( n=0; n<smal_slist_get_pool_cnt(&sl); ++n ) {
	int a;
	result = smal_slist_get_element_at( &sl, n, &a, (uint8_t)sizeof(a));
	if(result) {
	    fprintf( stderr, "smal_slist_get_element_at fail, strerror=%s\n", strerror(result));
	    goto out;
	}
	fprintf( stderr, "%c", a);
    }
    fprintf( stderr, "\n");

    fprintf( stderr, "err test\n");
    result = smal_slist_get_element_at( &sl, n, &a, (uint8_t)sizeof(a));
    if(result) {
	fprintf( stderr, "smal_slist_get_element_at(%u) fail, strerror=%s\n", n, strerror(result));
    } else {
	fprintf( stderr, "smal_slist_get_element_at fail, ERRNG\n");
	goto out;
    }

    fprintf( stderr, "pop\n");
    for( n=0; smal_slist_is_empty(&sl) == 0; ++n) {
	int a;
	result = smal_slist_front( &sl, &a, (uint8_t)sizeof(a));
	if(result) {
	    fprintf( stderr, "smal_slist_front fail, strerror=%s\n", strerror(result));
	    goto out;
	}
	result = smal_slist_pop(&sl);
	if(result) {
	    fprintf( stderr, "smal_slist_pop fail, strerror=%s\n", strerror(result));
	    goto out;
	}
	fprintf( stderr, "%c", a);
    }
    fprintf( stderr, "\n");
    fprintf( stderr, "poolcnt = %llu\n", (long long)smal_slist_get_pool_cnt(&sl));

    result = smal_slist_destroy(&sl);
    if(result) {
	fprintf( stderr, "smal_slist_destroy fail, result = 0x%x\n", result);
	goto out;
    }
    status = 0;
out:
    return status;

}





#define HEAPSZ 4096
static uint8_t vheap[HEAPSZ];

int main()
{
    std::cout << "Hello World!\n";

    smal_mallocater_t m;
    uint8_t *b[10], *c[10];

    std::cout << "Hello World!\n";

    test_slist();

    std::cout << "Hit Enter!\n";
    fgetc(stdin);
	
    smal_mallocater_init_obj( &m, vheap, HEAPSZ);

  for(;;) {
    for(uint8_t n=1; n<10; ++n) {
	b[n] = (uint8_t*)smal_mallocater_alloc_with_obj( &m, n);
	memset( b[n], n, n);
    }
    _smal_mallocater_dump_region_list(&m);

    smal_mallocater_free_with_obj(&m, b[2]);
    smal_mallocater_free_with_obj(&m, b[3]);
    b[2] = b[3] = NULL;

    for(uint8_t n=1; n<10; ++n) {
	c[n] = (uint8_t*)smal_mallocater_alloc_with_obj( &m, n);
	memset( c[n], n, n);
    }
    _smal_mallocater_dump_region_list(&m);

    for(uint8_t n=1; n<10; ++n) {
	if( NULL != b[n]) {
	    smal_mallocater_free_with_obj(&m, b[n]);
	}
	if( NULL != c[n]) {
	    smal_mallocater_free_with_obj(&m, c[n]);
	}
    }
    _smal_mallocater_dump_region_list(&m);
  }

  fgetc(stdin);

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
