#include <stdio.h>

/* ポインタのサイズ */
int main() {
	printf("int *: %zuバイト\n", sizeof(int *));
	printf("char *: %zuバイト\n", sizeof(char *));
	printf("float *: %zuバイト\n", sizeof(float *));
	return 0;
}

/*
 * メモ：
 * どのポインタも同じバイト数だが、コンパイラがアドレスを読んだ先の何バイトを読めばいいか理解するために別の型にする必要がある。
 * ポインタに対して演算したときに何バイト進めばいいかも型によって変わる。(intなら4byteなど)
 * 汎用のポインタとしてvoid *型がある。これにもアドレスを入れることができるが、ポインタ演算はできない。
 */

