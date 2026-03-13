#include <stdio.h>

/* strcpyなどはバッファオーバーフローが起きる場合があるので、snprintfの方がより安全(バッファサイズが指定できるので)。 */
/* フルネームを作成するプログラム */

int main(void)
{
	char first[] = "TARO";
	char last[] = "YAMADA";
	char full_name[100];

	snprintf(full_name, sizeof(full_name), "%s %s", first, last);
	printf("彼の名前は%sです。\n", full_name);

	return 0;
}
