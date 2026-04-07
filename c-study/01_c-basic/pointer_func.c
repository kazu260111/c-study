#include <stdio.h>

/* ポインタを使った関数の練習 */

void plus(int *p) {  /* int *p の部分は宣言なのでint p としない */
	++*p;  /* pに入っているアドレスの先に移動してから+1するので *p とする */
	return;
}

int main() {
	int count = 0;
	int *count_ptr;  /* int*型のアドレスを入れる場所を用意、countという名前だと同じ名前の変数になるのでptrとつける */
	count_ptr = &count;  /* *count_ptrと書かないことに注意、count_ptrにはアドレスを入れる */

	plus(count_ptr);  /* count_ptrをわざわざ用意しなくても最初から&countでよい */

	printf("%dは1になるはずです。\n", count);
	return 0;
}


