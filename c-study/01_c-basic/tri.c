#include <stdio.h>
/* 「C実践プログラミング」p128のコードを参考にした三角形の面積を計算するプログラム */

/*>>> 三角形の面積を計算する関数 <<<*/
float tri_calc(float width, float height) {
	float area;  /* 三角形の面積 */
	area = height * width / 2;  /* 三角形の面積は底辺×高さ÷２ */
	return area;
}

int main() {
/*>>> 変数の定義 <<<*/
	char line[100];  /* ユーザの入力を受け取る配列 */
	float height;  /* 三角形の高さ */
	float width;  /* 三角形の底辺の長さ */
	float area;  /* 三角形の面積 */

/*>>> ユーザ入力の処理 <<<*/
	printf("三角形の底辺と高さを半角スペース区切りで入力してください。（入力例：1.5 4)\nqまたは0でキャンセルします。\n");
	if (fgets(line, sizeof(line), stdin) == NULL) {
		printf("入力がありませんでした。プログラムを終了します。\n");
		return 1;
	}

/*>>> qが入力された場合終了する処理 <<<*/
	char cmd;  /* qかどうか確認するため1バイトの変数を準備 */
	if (sscanf(line, " %c", &cmd) == 1 && cmd == 'q') {
		printf("プログラムを終了します。\n");
		return 0;
	}

/*>>> 入力の検証 <<<*/
	int check_flag = sscanf(line, " %f %f", &width, &height);  /* sscanfの戻り値を検査するためにフラグを用意 */

	if (check_flag >= 1 && width == 0) {  /* ユーザが0とだけ入力した場合に終了する処理。フラグを検査せずに単純にwidth == 0だけを書いてしまうと未定義動作が起きてしまう場合があるので注意 */
		printf("プログラムを終了します。\n");
		return 0;
	}
	else if (check_flag != 2) {
		printf("不正な入力です。プログラムを終了します。\n");
		return 1;
	}
	else if (width < 0 || height < 0) {
		printf("底辺と高さは正の数を入力してください。プログラムを終了します。\n");
		return 1;
	}

/*>>> 三角形の面積の計算 <<<*/
	area = tri_calc(width, height);

/*>>> 計算結果の表示 <<<*/
	printf("三角形の面積は%fです。\n", area);

	return 0;
}
	



