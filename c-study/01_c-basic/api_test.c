/* 見本のコードをほぼそのまま書き写して練習 */
/* 外部API(gutendex)を使用して、本のタイトルと文章の一部を取得 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>  /* httpリクエストを出す */
#include <cjson/cJSON.h>  /* 届いた文字列をjsonデータとして処理 */

struct MemoryStruct {
	char *memory;  /* 届いたデータを保存する場所 */
	size_t size;  /* 現在のデータの大きさ */
};

/* WriteMemoryCallback関数の引数
 * void *contents  - 届いたデータのポインタ
 * size_t size  - 一個のデータのサイズ
 * size_t nmemb  - 届くデータの個数
 * void *userp  - ユーザーが決めたポインタ(データの送り先)
 */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	/* 届いたデータサイズを計算 */
	size_t realsize = size * nmemb;
	/* キャストをする(コールバック関数は引数と戻り値が指定されているので、関数の中で実際に使う型に変更する */
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;
	/* reallocは現在のメモリの場所と、最終的に必要なメモリサイズを引数にする。realsizeは\0がないので、余分に1バイト確保 */
	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
	/* エラーなら0を返す */
	if (ptr == NULL) return 0;
	/* 連続したメモリが取れない場合、別の場所のメモリを確保し直すことがあるのでmem->memoryを更新する */
	mem->memory = ptr;
	/* データの保存先にcontentsの先のデータを保存する */
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	/* mem->sizeを今回届いたデータサイズだけ増やす */
	mem->size += realsize;
	/* C言語で扱うために、終端文字を入れる */
	mem->memory[mem->size] = 0;
	/* 戻り値は今回処理したデータサイズで固定 */
	return realsize;
}

int main(void) {
	/* curlの通信で使うポインタ */
	CURL *curl;
	/* 通信結果を保存する */
	CURLcode res;
	/* データを管理する構造体変数 */
	struct MemoryStruct chunk;
	/* とりあえず1バイト確保 */
	chunk.memory = malloc(1);
	/* 現在のデータサイズは0 */
	chunk.size = 0;
	
	/* 開始処理 */
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	/* curlがNULLでないなら */
	if (curl) {
		/* ここからcurl_easy_performを実行するときの設定をする */
		/* 通信処理 */
		curl_easy_setopt(curl, CURLOPT_URL, "https://gutendex.com/books/?search=Alice");
		/* データが届いたときに渡す先(コールバック関数)のアドレスを渡す */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		/* データの保存先のポインタを渡す */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		
		/* 通信の実行 */
		/* 通信するたびにコールバック関数が実行され、reallocで拡張されたmem_memory(main関数でのchunk)の新しい部分に受け取ったデータが追加される。 */
		res = curl_easy_perform(curl);

		if (res == CURLE_OK) {
			/* jsonの解析 */
			cJSON *json = cJSON_Parse(chunk.memory);
			if (json) {
				/* CaseSensitiveは大文字小文字を区別する(その方が処理が軽いし確実) */
				cJSON *results = cJSON_GetObjectItemCaseSensitive(json, "results");
				/* resultsの最初の一冊目だけ選択 */
				cJSON *book = cJSON_GetArrayItem(results, 0);
				cJSON *title = cJSON_GetObjectItemCaseSensitive(book, "title");

				if (cJSON_IsString(title) && (title->valuestring != NULL)) {
					printf("取得したタイトル: %s\n", title->valuestring);
				}
				/* 本の内容のURLを探す */
				cJSON *formats = cJSON_GetObjectItemCaseSensitive(book, "formats");
				cJSON *text_url_item = cJSON_GetObjectItemCaseSensitive(formats, "text/plain; charset=us-ascii");

				/* 本の中身のURLが見つかったらもう一度通信 */
				if (cJSON_IsString(text_url_item) && (text_url_item->valuestring != NULL)) {
						char *text_url = text_url_item->valuestring;
						printf("テキストをダウンロードしています・・・\n");
						struct MemoryStruct book_content;
						book_content.memory = malloc(1);
						book_content.size = 0;
						/* curlの設定 */	
						curl_easy_setopt(curl, CURLOPT_URL, text_url);
						curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&book_content);
						/* 通信実行 */
						CURLcode dl_res = curl_easy_perform(curl);
						if (dl_res == CURLE_OK) {
							/* 文字数が500未満なら実際の文字数に、500以上なら500文字を表示する */
							int display_length = (book_content.size < 500) ? book_content.size : 500;
							printf("===== 本の冒頭部分(%d文字) =====\n", display_length);
							for (int i = 0; i < display_length; ++i) {
								putchar(book_content.memory[i]);
							}
							printf("\n\n===== 終わり =====\n");
						}
						else {
							printf("本の内容が見つかりませんでした。\n");
						}
						/* 使用したメモリの解放 */
						free(book_content.memory);
				}				
				/* 終了(メモリの解放) */
				cJSON_Delete(json);
			}
	 			
		}

		curl_easy_cleanup(curl);
		free(chunk.memory);
	}

	curl_global_cleanup();
	return 0;
}

/* 実行結果 */
/*~$ ./api_test 
 * 取得したタイトル: Alice's Adventures in Wonderland
 * テキストをダウンロードしています・・・
 * ===== 本の冒頭部分(500文字) =====
 * *** START OF THE PROJECT GUTENBERG EBOOK 11 ***
 * 
 * [Illustration]
 * 
 * 
 * 
 * 
 * Alice’s Adventures in Wonderland
 * 
 * by Lewis Carroll
 * 
 * THE MILLENNIUM FULCRUM EDITION 3.0
 * 
 * Contents
 * 
 *  CHAPTER I.     Down the Rabbit-Hole
 *  CHAPTER II.    The Pool of Tears
 *  CHAPTER III.   A Caucus-Race and a Long Tale
 *  CHAPTER IV.    The Rabbit Sends in a Little Bill
 *  CHAPTER V.     Advice from a Caterpillar
 *  CHAPTER VI.    Pig and Pepper
 *  CHAPTER VII.   A Mad Tea-Party
 *  CHAPTER VIII.  The Queen’s Croquet-Ground
 *  CHAPTER IX.   
 * 
 * ===== 終わり =====
 *
 */
