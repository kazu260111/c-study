#include <stdio.h>
/* 「C実践プログラミング」p98のプログラムを参考にした簡易計算機（整数のみ） */

/*>>> 必要な変数の定義 <<<*/
int main() {
char line[100];  /* 入力用 */
int result;  /* 計算結果（整数のみ）*/
char operator;  /* ユーザが指定した演算子 */
int value;  /* ユーザが指定した、resultに対して計算する数値 */
int check_flag;  /* ユーザの入力を検証するための変数 */

/*>>> 最初の数値を代入する処理 <<<*/
	result = 0;  /* resultを初期化 */
	printf("最初の整数値を入れてください。入力した数値がresultの初期値になります。\n");
	/* 何も入力されない場合エラーで終了 */
	if (fgets(line, sizeof(line), stdin) == NULL) {
		       return 1;
		       }	       
	sscanf(line, "%d", &result);

/*>>> ユーザ入力を受け取る処理 <<<*/
	while (1) {  /* breakされるまで無限ループ、連続して計算できる */
		printf("現在のresultの値は%dです。\n", result);
		printf("演算子と数値を入力してください。計算を終了する場合は演算子をqにしてください。演算子は+, - *のみ使用可能です。(例：+ 1)\n");
		if (fgets(line, sizeof(line), stdin) == NULL) {
				printf("入力がありませんでした。プログラムを終了します。\n");
				break;
				}		
		check_flag = sscanf(line, " %c %d", &operator, &value);
		/* 終了時の分岐（入力形式のチェックよりも先に分岐しないと、ユーザがqだけを入力した場合に無限ループしてしまうのでここに分岐を移動した）*/
/*>>> 演算子の確認処理 <<<*/
		if (check_flag >= 1 && (operator == 'q' || operator == 'Q')) {  /* qまたはQでプログラムを終了。入力がなかった場合に未定義動作が起きないよう、check_flag >= 1 を追加した。（check_flag >= 1 が偽なら短絡評価で未定義動作が実行されない） */
			printf("簡易計算機を終了します。\n");
			break;
		}
		else if (check_flag != 2) {  /* もし入力を正しく読めなかった場合 */
			printf("入力形式が間違っています。計算をキャンセルします。\n");
		}
		else if (operator == '+') {
			result = result + value;
		}
		else if (operator == '-') {
			result = result - value;
		}
		else if (operator == '*') {
			result =  result * value;
		}
		else {
			printf("不正な演算子です。計算をキャンセルします。\n");
		}
	}
	return 0;
}


