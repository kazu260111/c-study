/* 注意：このプログラムは実験用プログラムです。意図的なバグが含まれています。 */
#include <stdio.h>
#include <stdlib.h>

int main(void){
	int *ptr = malloc(sizeof(int));  /* intサイズのメモリを確保 */
	*ptr = 1;  /* 確保したメモリに1を書き込む */
	free(ptr);  /* 確保したメモリを解放 */
	*ptr = 2;  /*>>> 解放したメモリに2を書き込む(危険) <<<*/
	printf("%d\n", *ptr);  /* 解放したあと2を書き込んだはずのメモリにアクセスする */
	/* ここから追加のプログラム */
	int *ptr_2 = calloc(1, sizeof(int));  /* 新しくメモリを確保 */
	printf("%d\n", *ptr);  /* もう一度2が書き込まれているはずのメモリにアクセスする */
	free(ptr_2);
	return 0;
}


