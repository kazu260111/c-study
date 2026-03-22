/* 構造体の復習 2 */
/* 誕生日の人の年齢を１加算するプログラム */

#include <stdio.h>

typedef struct Person {
	char name[30];
	int age;
} Person;

void birthday(Person *p) {  /* 引数となったPerson型の変数の最初のアドレス（今回だとnameの最初）を受け取る */ 
	++(*p).age;  /* 演算子の処理優先度があるため、カッコをつける必要がある。（これよりも同じ意味のアロー演算子を使った方が良い）*/
	return;  /* 戻り値がないので書かなくてよい */
}
int main() {
	Person a = {"SUZUKI TARO", 20};
	printf("%sさんは%d歳でしたが、", a.name, a.age);
	birthday(&a);
	printf("今日で%d歳になりました。\n", a.age);
	return 0;
}





