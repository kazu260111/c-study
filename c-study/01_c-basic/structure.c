/* 構造体の復習 */

#include <stdio.h>

typedef struct Person
{
	char name[30];
	int age;
} Person;

int main() {
	Person a = {"SUZUKI TARO", 17 };
	Person b = {"YAMADA ITIRO", 20};

	printf("%sさんの年齢は%dです。\n", a.name, a.age);
	printf("%sさんの年齢は%dです。\n", b.name, b.age);
	
	return 0;
}



