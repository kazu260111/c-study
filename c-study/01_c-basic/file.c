/* ファイルを開ける練習 */

#include <stdio.h>

int main() {
	FILE *fp = NULL;
	char line[128];

	fp = fopen("test.txt", "r");  /* fopen関数は戻り値として開けたファイルのアドレスを返す */
	if (fp == NULL) {  /* fopen関数はファイルが開けなければNULLを返す */
		printf("ファイルが開けませんでした。\n");
		return 1;
	}

	while (fgets(line, sizeof(line), fp)) {  /* FILE型にはこれまで読み込んだファイルの位置を記憶する領域があるので、同じアドレス（fp）を何度も読み込んでよい */
	       printf("%s", line);
	}
	fclose(fp);  /* fclose関数は閉じたいファイルのアドレスを指定する */
	return 0;
}

