#include <stdio.h>

/* 構造体の練習 */

/*>>> 構造体の定義、年齢と名前と母親をメンバにする <<<*/
struct Person {
	int age;
	char name[256];
	struct Person *mother;
};

int main() {
       	struct Person b = {40, "Alice", NULL};  /* 依存関係があるのでこちらを先に定義 */
	struct Person a = {10, "Bob", &b};  

	printf("%sの母親は%sです。\n", a.name, (*a.mother).name);  /* 処理順はa.mother, *a.mother, (*a.mother).name の順番。*よりも先に.が処理される。 */

	return 0;
}
