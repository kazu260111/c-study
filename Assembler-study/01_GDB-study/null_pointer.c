#include <stdio.h>
/* 注意：このプログラムはクラッシュします */
int null_pointer_func(void){
	int *p = NULL;
	/* ここでNULLポインタ参照が起きる */
	*p = 1;
	return 0;
}
int main(void) {
	printf("プログラムが実行されました\n");
	null_pointer_func();
	printf("この文章は表示されません\n");
	return 0;
}


