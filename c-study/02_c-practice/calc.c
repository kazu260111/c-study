/**************************************************
 *                                                *
 *    　　　　　　 簡易四則演算機　               *
 *                                                *
 **************************************************/

/*--- 仕様 ---*/
/* 1.プロンプトを表示し、ユーザーから1つ目の数字の入力を受け取る（計算式を表示するエリアを準備し、そこに書き足していく）
 * 2.演算子を選択するメニューを表示し、ユーザーから演算子の番号を受け取る(+,-,*,/)
 * 3.プロンプトを表示し、ユーザーから2つ目の数字の入力を受け取る
 * 4.計算式と計算結果を表示し、ユーザーにこの結果を利用して次の計算を続けるか、計算結果を破棄してやり直すか、、プログラムを終了するかを選択させる
 */

#include <stdio.h>
#include <stdlib.h>


static char line[100];  /* ユーザー入力を一時的に受け取るための配列(グローバル) */

/*>>> 選択できる演算子を表示し、ユーザーの選択を戻り値とする関数の定義 <<<*/
int op_menu(void) {
	printf("演算子を選択してください。\n");
	printf("1) +\n2) -\n3) *\n4) /\n5) 計算をやめる\n");
	fgets(line, sizeof(line), stdin);
	return atoi(line);  /* ユーザーが入力した数値を戻り値とする */
}

/* ユーザーの演算子選択をチェックし、演算子以外が選ばれた場合は終了させる関数の定義 */
void op_check(int op_menu_flag) {
	if (op_menu_flag != 1 && op_menu_flag != 2 && op_menu_flag != 3 && op_menu_flag != 4 && op_menu_flag != 5) {
		printf("不適切な入力です。簡易四則演算機を終了します。\n");
		exit (1);
	}
	else if (op_menu_flag == 5) {
		printf("簡易四則演算機を終了します。\n");
		exit (0);
	}
}


int main(void) {
	/*>>> 使用する変数と配列の準備 <<<*/
	double num1 = 0;  /* ユーザーの入力する1つ目の数字 */
	double num2 = 0;  /* ユーザーの入力する2つ目の数字 */
	double result = 0; /* 計算結果を入れる変数 */
	int op_menu_flag = 0;  /* ユーザーが選んだ演算子を記録するフラグ */
	char op[4] = {'+', '-', '*', '/'} ;  /* 演算子を表す文字列を格納する配列 */

	/*>>> 起動時のアナウンス <<<*/
	printf("簡易四則演算機を起動しました。\n");
	
	/*>>> 1つ目の数字の入力を受け取る <<<*/
	printf("1つ目の数字を入力してください。\n");  /* プロンプト */
	fgets(line, sizeof(line), stdin);  /* 数字の入力を受け取る */
	num1 = atof(line);  /* num1に1つ目の数字を代入 */
	

	/* 連続して計算できるようwhile文でループ。計算を終了する場合breakする */
	while (1) {
		/*>>> 演算子の選択 <<<*/
		printf("計算式：%f\n", num1);  /* 現在の計算式の表示 */
		op_menu_flag = op_menu();  /* 選択できる演算子を表示し、ユーザーが選択した番号をフラグに入れる */
		op_check(op_menu_flag); 	/* ユーザーの演算子選択をチェックし、演算子以外が選ばれた場合は終了させる */
		/*>>> 2つ目の数字の入力を受け取る <<<*/
		printf("計算式：%f %c \n", num1, op[op_menu_flag - 1]); /* 現在の計算式の表示 */
		printf("2つ目の数字を入力してください。\n");  /* プロンプト */
		fgets(line, sizeof(line), stdin);  /* 数字の入力を受け取る */
		num2 = atof(line);  /* num2に2つ目の数字を代入 */
		
		/*>>> 計算処理 <<<*/
		switch (op_menu_flag) {
			case 1:
				result = num1 + num2;
				break;
			case 2:
				result = num1 - num2;
				break;
			case 3:
				result = num1 * num2;
				break;
			case 4:
				result = num1 / num2;
				break;
			default:
				printf("致命的なエラーが発生しました。\n");
				exit (1);
		}

		/*>>> 計算結果の表示 <<<*/
		printf("計算式：%f %c %f\n", num1, op[op_menu_flag - 1], num2); 
		printf("計算結果は%fです。\n", result);
		
		/*>>> 計算を続けるかの確認 <<<*/
		printf("続けて計算しますか？[y/n]   ");
		fgets(line, sizeof(line), stdin);
		if (line[0] == 'y' || line[0] == 'Y') {  /* 注意：先頭の一文字しかチェックしていないので、yyやyeahなどでも条件を満たす。*/
			num1 = result;  /* 今回の計算結果をnum1に入れて、これと次にユーザーが入力するnum2とで計算する */
			/* while文の最初（演算子の選択）まで戻る */
		}
		else {
			break;  /* while文を終了させて終了処理に移行 */
		}
	}

	/* 終了処理 */
	printf("簡易四則演算機を終了します。\n");
	return 0;
}
