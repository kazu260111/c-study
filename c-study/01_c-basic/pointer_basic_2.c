#include <stdio.h>

/* ポインタ演算の練習 */

int main() {
	int a = 0;
	int *a_ptr = &a;
	printf("int a のアドレスは%p\n", a_ptr);  /* %pはvoid *型を受け取る仕様なので(void *)a_ptr としてキャストするとよりよい */
	printf("int a のポインタに1を加えると%p\n", a_ptr + 1);  /* 4バイト進む */

	char b = 0;
	char *b_ptr = &b;
	printf("char b のアドレスは%p\n", b_ptr);
	printf("char b のポインタに1を加えると%p\n", b_ptr + 1);  /* 1バイト進む */
	return 0;
}


