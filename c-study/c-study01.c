/************************************************************
 *                                                          *
 * 　　　　　　　　　　　 C言語の復習                       *
 *                                                          *
 ************************************************************/




#if 0

/* 数字を入れ替える(ポインタ操作)*/

#include <stdio.h>

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}


int main(void) 
{
	int a = 2;
	int b = 3;
	printf("a=%d, b=%d\n", a, b);
	printf("今から入れ替える関数を呼び出します。\n");
	swap(&a, &b);
	printf("a=%d, b=%d", a, b);

	return 0;
}

#endif


#if 0
/*構造体*/

#include <stdio.h>

struct Person
{
	char name[16];
	int age;
};

int main(void)
{
	struct Person a = {"SUZUKI TARO", 17};
	struct Person b = {"SATO HANAKO", 19};

	printf("名前:%s、年齢%d\n", a.name, a.age);
	printf("名前:%s、年齢%d\n", b.name, b.age);

	return 0;
}

#endif



#if 0
/*構造体とポインタ、typedef*/


/*誕生日の人の年齢を1増やすプログラム*/

#include <stdio.h>

typedef struct Person
{
	char name[16];
	int age;
} Person;

void print_info(Person p)  /*Person型として引数を受け取る*/
{
	printf("名前:%s, 年齢:%d\n", p.name, p.age);
}

void birthday(Person *p)  /* pにはアドレスが入る*/
{
	(*p).age++;  /* 括弧がないと演算子の処理順からドット(.)が先に処理されるため、p.ageの部分を先に処理してしまう*/
}

int main(void)
{
	Person p = {"SUZUKI TARO", 17};
	print_info(p);  /*現在の年齢を画面に出力、内部では構造体をコピー(20byte)*/
	birthday(&p);  /*誕生日による年齢加算処理、内部ではアドレスだけを渡している*/
	print_info(p);  /*加算した年齢を画面に出力*/

	return 0;
}

#endif

#if 0
/* const修飾子とアロー演算子、配列*/
/* C言語では構造体を使うとき基本的にポインタを渡す(const修飾子でメンバを変更しないようにできる)*
 * 構造体全体をコピーするのはメモリの無駄になることが多い                                      */



/*同じ誕生日の二人の年齢を加算する*/
#include <stdio.h>

typedef struct Person
{
	char name[16];
	int age;
} Person;

void print_info(const Person *p0, const Person *p1)
{
	printf("名前:%s、年齢:%d\n", (*p0).name, (*p0).age);
	printf("名前:%s、年齢:%d\n", (*p1).name, (*p1).age);
}

void birthday(Person *p0, Person *p1)
{
	(*p0).age++;  /* p0->age++とも書ける*/
	(*p1).age++;
}

int main(void)
{
	Person p[] = { {"SUZUKI TARO", 17}, {"SATO HANAKO", 19} };

	print_info(&p[0], &p[1]);
	birthday(&p[0], &p[1]);
	print_info(&p[0], &p[1]);

	return 0;
}

#endif


#if 0

/* ポインタと配列*/

#include <stdio.h>

typedef struct Person
{
	char name[16];
	int age;
	struct Person *mother;  /*母親の名前を表示させるポインタ*/
				/*この時点ではtypedef Personが終わっていないのでstruct Personと入力する*/
} Person;
int main(void)
{
	Person p0 = {"SUZUKI AKIKO", 44, NULL};  /* ポインタが設定されないときはNULLを入れる*/
	Person p1 = {"SUZUKI TARO", 17, &p0};  /* 母親がp0なのでアドレスを入れておく*/

	printf("%sの母親は%s\n", p1.name, (*p1.mother).name);  /* p1.mother->nameでもよい*/
	/* p1.motherに入っているアドレス先のPerson型のname部分を呼び出している*/
	/* 処理順はp1.mother -> *p1.mother -> (*p1.mother).name */

	return 0;
}

#endif












