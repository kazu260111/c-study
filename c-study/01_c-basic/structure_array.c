#include <stdio.h>
/* 構造体配列の練習 (「C実践プログラミング」p182を参考にしたプログラム)*/

/*
 * マラソンにおける周回と時間を記録するプログラム
 */

#define MAX_LAPS 5  /* 最大の周回数を変えるときはここを変える */

/*>>> 構造体の定義 <<<*/
	struct run_time {
		int hour;  /* 時間 */
		int minute;  /* 分 */
		int second;  /* 秒 */
	};

int main() {
/*>>> ユーザ入力の受付による周回記録の作成 <<<*/
	struct run_time lap[MAX_LAPS];  /* run_time型の配列を作り、周回の記録を保存 */
	int laps = 0;  /* 現在の周回数 */
	char line[128];  /* ユーザ入力受付用 */
	
	printf("マラソン周回記録用プログラムを起動します。一周ごとに入力してください。\n");
	while (laps < MAX_LAPS) {
		printf("\n現在 %d 周完了しました。今回の周回の記録を入力してください。(半角スペース区切り、h m s の形式、qで周回記録表示に進む)\n", laps + 1);
		printf("例:10分10秒の場合、 0 10 10 と入力\n");
		
		/* 入力がない場合 */		
		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("入力がありませんでした。もう一度入力してください。\n");
			continue;
		}

		/* qが入力された場合 */
		if (line[0] == 'q' || line[0] == 'Q') {
			printf("記録を中止し、周回記録の表示に進みます。\n");
			break;
		}

		/* 時間、分、秒をすべて読み取れなかった場合 */
		if (sscanf(line, " %d %d %d", &lap[laps].hour, &lap[laps].minute, &lap[laps].second) != 3) {
			printf("不正な入力です。時間、分、秒をすべて半角スペース区切りで入力してください。\n");
			continue;
		}

		++laps;
	}

/*>>> 周回記録の表示 <<<*/
	/* 記録がない場合 */
	if (laps <= 0) {
		printf("記録がありません。プログラムを終了します。\n");
		return 1;
	}

	printf("周回が完了しました。記録を表示します。\n\n");
	int print_laps = 0;
	
	while (print_laps < laps) {
		printf("%d回目の周回の記録は%d時間%d分%d秒です。\n", print_laps + 1, lap[print_laps].hour, lap[print_laps].minute, lap[print_laps].second);
		++print_laps;
	}

	/* 終了メッセージ */
	printf("\nすべての記録を表示しました。プログラムを終了します。\n");
	return 0;
}




		





