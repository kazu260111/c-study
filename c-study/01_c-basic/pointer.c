/* ポインタの練習 */

#include <stdio.h>

int main(void) {
	int i = 5;
	int *p = &i;
	printf("変数iのアドレスは%pです。\n", (void *)&i);  /* %pはvoid *型を受け取る */
	printf("変数iのアドレスは%pです。\n", (void *)p);
	printf("int *型のサイズは%zuです。\n", sizeof(int *));  /* 32bit環境だと%dを使っても動作するが、移植性などを考えると%zuにすべき */

	double d = 10;
	double *q = &d;
	printf("変数dのアドレスは%pです。\n", (void *)&d);
	printf("変数dのアドレスは%pです。\n", (void *)q);
	printf("double *型のサイズは%zuです。\n", sizeof(double *));

	return 0;
}

