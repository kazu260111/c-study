/* fgets, sscanfの練習 */

#include <stdio.h>
#include <string.h>
int main(void)
{
	int a;
	int b;
	int c;
	char line[100];
	printf("数値を３つ入力してください。\n");
	fgets(line, sizeof(line), stdin);  /* 標準入力から読み取る */
	if (sscanf(line, "%d %d %d", &a, &b, &c) == 3) /* sscanfの戻り値は読み込めた変数の数なので、正しく３つ読み込めたのか確認する */ 
	{
		printf("入力した３つの数字をかけ合わせると、%dです。\n", a*b*c);
	}
	else
	{
		printf("エラーが発生しました。数字は３つだけ入れ、半角スペースで区切ってください。\n");
	}
	
	return 0;
}	
