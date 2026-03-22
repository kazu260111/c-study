/* ポインタを含む構造体の練習 */

#include <stdio.h>

typedef struct Person {
	char name[30];
	int age;
	struct Person *mother;  /* この時点でtypedefが終わっていないのでPersonなどと省略してはいけない */
} Person;

int main() {
	Person p[] = {{"SUZUKI TARO", 15, &p[1]}, {"SUZUKI HANAKO", 40, NULL}};
       	printf("%sの母親は%sです。\n", p[0].name, (*p[0].mother).name);  /* 演算子の処理順に注意。最初にp[0].mother、次に*p[0].motherを処理し、最後に(*p[0].mother).nameを処理する */

	return 0;
}
