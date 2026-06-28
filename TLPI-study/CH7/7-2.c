#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#define BRK_UP 139264


/* 7-2 malloc()とfree()の実装 */

/*
 * メモ:
 * malloc()はヘッダ(確保したメモリのサイズ)と呼び出し側が使える領域を確保する
 * my_malloc()は引数で受けたメモリサイズの領域分以上の空き領域を空きリストから探す
 * - 確保したいメモリと同じサイズの空き領域があればそれを使用
 * - 確保したいメモリより大きな空き領域があれば候補として保留し、探索を続ける(同じサイズを探す)
 *   - もし同じサイズがなければ、現在の候補を分割して利用し、残った領域は空きリストにつなぎなおす
 * 空きリストに必要な領域以上の空き領域がなければ、sbrk(0)で現在のプログラムブレークを取得、
 * sbrk(BRK_UP)でプログラムブレークを上げて動いたプログラムブレーク分を空きリストに追加
 * - 空きリストに追加時、キャストして各フィールドに要素を入れる
 * -> 空きリストに必要な領域がある状態になるので分割して利用できる
 *
 * 空きリストから見つけた領域のnext開始部分のアドレスを呼び出し元に渡せばよい
 *
 */


struct free_list {
	/* ヘッダ */
	size_t size;  /* 呼び出し元が使えるの領域のサイズ、ヘッダ(自分)は含まない 8byte */
	/* 呼び出し元が使う領域、malloc()で貸し出されると上書きされる領域 */
	struct free_list *next;  /* リストの次の要素 */
	struct free_list *prev;  /* リストの前の要素 */
	/* この後にも呼び出し元に渡す領域が続く */
};

/* 空きリストの先頭のポインタ */
static struct free_list *lists;

/* 渡されたlistをつなぎなおし、my_mallocを呼び出した関数に渡すアドレスを返す */
struct free_list **list_reconnect(struct free_list *list) {
	/*>>> 現在のlistの領域を使うので、空きリストをつなぎなおす <<<*/
	/* リストの両端でないなら一個手前のリストのnextと一個後ろのprevを編集 */
	if (list->prev && list->next) {
		list->prev->next = list->next;
		list->next->prev = list->prev;
	}
	/* リストの先頭(他に要素あり)なら一個後ろのprevとlistsを編集 */
	if (!list->prev && list->next) {
		list->next->prev = NULL;
		lists = list->next;
	}
	/* リストの最後尾(他に要素あり)なら一個手前のnextを編集 */
	if (list->prev && !list->next) {
		list->prev->next = NULL;
	}
	/* 現在のlistが唯一の要素の場合 */
	if (!list->prev && !list->next) {
		lists = NULL;
	}
	/*>>> 呼び出し元にアドレスを返す <<<*/
	return &list->next;
}

/* 引数size: 呼び出し元が使える領域のサイズ(ヘッダは含まない)*/
void *my_malloc(size_t size) {
	/* sizeが0ならNULLを返す */
	if (!size) {
		return NULL;
	}
	/* 実際に呼び出し元に渡すヘッダ込みのサイズ */
	size_t real_size;
	/* mallocは8byte境界になるようにする必要があるので丸めあげる */
	real_size = ((size + 8) + 0b111) & ~0b111;
	/* 実際に呼び出し元に渡すヘッダを含まないサイズ */
	size_t fix_size = real_size - 8;
	/*>>>>>> 空きリストを先頭から走査 <<<<<<*/
	struct free_list *list;
	/* サイズの大きい領域を候補としてキープ */
	struct free_list *list_op = NULL;
	for (list = lists; list; list = list->next) {
		/*>>> サイズの比較 <<<*/
		/* サイズが足りないとき */
		if (fix_size > list->size) {
			continue;
		}
		/* サイズが一致するとき */
		if (fix_size == list->size) {
			return list_reconnect(list);
		}
		/* 候補がすでに決まっているなら候補の更新はせず領域サイズが一致するものが見つかるまでループ */
		if (list_op) {
			continue;
		}
		/* 候補が決まっていなくてサイズが大きいとき */
		if (fix_size < list->size) {
			list_op = list;
			continue;
		}
	}
	/* forループ終了(ピッタリサイズの領域があればそれを返して終わる */
	/*>>>>>> 大きめの領域があれば分割してリストをつなぎなおす <<<<<<*/
	if (list_op) {
		list = list_op;
		/* 分割するには余りが16バイト必要 */
		if (list->size - fix_size < 16) {
		        /* 分割せずにそのまま使う */
			return list_reconnect(list);
		}
		/*>>> 分割する場合 <<<*/
		/* 呼び出し元に渡す領域の設定 */
		list->size = fix_size;
		/* リストに残る領域を設定 */
		struct free_list *rest_area;
		rest_area = (struct free_list *)((char *)list + real_size);
		rest_area->size = list_op->size - fix_size - 8;
		/* 先頭につなぎなおす */
		rest_area->next = lists;
		rest_area->prev = NULL;
		if (lists) {
			lists->prev = rest_area;
		}
		/* リスト先頭を更新 */
		lists = rest_area;
		return &list->next;		
	}

	/*>>>>>> 使える領域がリストに一つも見つからなかったらプログラムブレークを上げリストに追加 <<<<<<*/
	/* 呼び出し元に渡すポインタ */
	struct free_list *new_area;
	/* 現在のプログラムブレークを取得 */
	new_area = sbrk(0);
	/* sbrk()失敗時 */
	if (new_area == (void *)-1) {
		fprintf(stderr, "sbrk()失敗: %s\n", strerror(errno));
		return NULL;
	}
	size_t up_size;
	/* プログラムブレークを上げる */
	for (up_size = 0; real_size > up_size; up_size += BRK_UP) {
		if (sbrk(BRK_UP) == (void *)-1) {
			fprintf(stderr, "sbrk()失敗: %s\n", strerror(errno));
			return NULL;
		}
	}
	/*>>>>>> 確保した領域を分割する <<<<<<*/
	/*>>> 分割して余っているリストに残す方をリストに登録する <<<*/
	/* リストに追加する領域(余る領域)のポインタ */
	struct free_list *new_list;
	/* char *でキャストしないと進みすぎるので注意 */
	new_list = (struct free_list *)((char *)new_area + real_size);
	/* ヘッダのサイズ情報を記録 */
	new_list->size = up_size - real_size;
	/* リストの一番最初につなぐ */
	new_list->next = lists;
	new_list->prev = NULL;
	if (lists) {
		lists->prev = new_list;
	}
	/* リスト先頭を更新 */
	lists = new_list;
	return &new_area->next;
}

void my_free(void *ptr) {
}


int main(void) {
}



