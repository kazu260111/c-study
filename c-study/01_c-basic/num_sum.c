#include <stdio.h>

/**************************************************
 *                                                *
 *  入力された数字をすべて足し合わせるプログラム  *
 *                                                *
 **************************************************/

/*
 * 使い方：
 * ユーザが最大5個まで数字を入力すると、それをすべて足し合わせるプログラム
 *
 */

int main() {
/*>>> 変数の定義 <<<*/
	char line[256];  /* ユーザ入力用 */
	float num1, num2, num3, num4, num5;  /* ユーザ入力の数字用 */
	float sum;
/*>>> プロンプトとユーザ入力受付 <<<*/
	printf("足し合わせる数字をすべて入力してください。(最大5個、半角スペース区切り)\n");
	
	/* ユーザ入力がない場合プログラムを終了する */
	if (fgets(line, sizeof(line), stdin) == NULL) {  
		printf("入力がありませんでした。プログラムを終了します。\n");
		return 1;
	}
	
/*>>> 入力の検証と計算処理 <<<*/
	/* 読み取れた数字をフラグに保存 */
	int check_flag = sscanf(line, " %f %f %f %f %f", &num1, &num2, &num3, &num4, &num5);

	if (check_flag <= 0) {
		printf("数字が読み取れませんでした。数字と半角スペース以外を入力するとすべての数字を読み取れない場合があります。プログラムを終了します。\n");
		return 1;
	}
	else if (check_flag == 1) {
		printf("数字を2個以上入力してください。数字と半角スペース以外を入力するとすべての数字を読み取れない場合があります。\n");
		return 1;
	}
	else if (check_flag == 2) {
		sum = num1 + num2;
		printf("数字の合計は%fです。\n", sum);
	}
	else if (check_flag == 3) {
		sum = num1 + num2 + num3;
		printf("数字の合計は%fです。\n", sum);
	}
	else if (check_flag == 4) {
		sum = num1 + num2 + num3 + num4;
		printf("数字の合計は%fです。\n", sum);
	}
	else if (check_flag == 5) {
		sum = num1 + num2 + num3 + num4 + num5;
		printf("数字の合計は%fです。\n", sum);
	}
	else {
		printf("致命的なエラーが発生しました。\n");
		return 1;
	}
	
	return 0;
}

/* 
 * メモ：
 * 次はこのプログラムを改善して、任意の個数の入力を受け付けられるより洗練したバージョンを作成する予定
 *
 */



