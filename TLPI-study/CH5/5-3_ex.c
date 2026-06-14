/* 5-3 O_APPENDのアトミック性が必要な理由 */

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#define ATOMIC 3
#define NON_ATOMIC 4
int main(int argc, char *argv[]) {
	/* 引数のチェック */
	if (argc <= 2 || argc >= 5) {
		fprintf(stderr, "使い方: %s [(新規)ファイル名] [追加するバイト数] (O_APPENDを使わないなら第3引数としてxを入力)\n", argv[0]);
		return 1;
	}

	/* 引数のバイト数に対してstrtol実行時のエラー判定 */
	char *endptr;
	errno = 0;
	long val = strtol(argv[2], &endptr, 10);
	/* オーバーフロー */
	if (errno == ERANGE) {
		fprintf(stderr, "longの値の範囲外です: %s %s\n", argv[2], strerror(errno));
		return 1;
	}
	if (endptr == argv[2]) {
		fprintf(stderr, "数字が読めませんでした\n");
		return 1;
	}
	if (*endptr != '\0') {
		fprintf(stderr, "数字以外が含まれています\n");
		return 1;
	}
	if (val <= 0) {
		fprintf(stderr, "バイト数は正の数にしてください\n");
		return 1;
	}
	/* アトミック・非アトミックの分岐はフラグを先に設定する */
	int flags = O_CREAT | O_WRONLY | (argc == ATOMIC ? O_APPEND:0);
	int fd = open(argv[1], flags, 0644);
	if (fd == -1) {
		fprintf(stderr, "open()失敗: %s\n", strerror(errno));
		return 1;
	}
	/* 書き込み */
	char buf[] = {'1'};
	for (long w_count = 0; w_count < val; ++w_count) {
		if (argc == NON_ATOMIC) {
			if (lseek(fd, 0, SEEK_END) == -1) {
				fprintf(stderr, "lseek()失敗: %s\n", strerror(errno));
				close(fd);
				return 1;
			}
		}	
		ssize_t n_write = write(fd, buf, sizeof(buf));
		if (n_write == -1) {
			fprintf(stderr, "write()失敗: %s\n", strerror(errno));
			close(fd);
			return 1;
		}
		if (n_write < (ssize_t)sizeof(buf)) {
			fprintf(stderr, "バッファが全て書き込めませんでした: %zdバイト書込み\n", n_write);
			close(fd);
			return 1;
		}
	}
	/* 終了処理 */
	if (argc == ATOMIC) {
		printf("アトミックな書き込みが完了しました\n");
	}
	else if (argc == NON_ATOMIC) {
		printf("非アトミックな書き込みが完了しました\n");
	}
	close(fd);
	return 0;
}

/*
 * 実行結果
 * $ ./5-3.out test_5-3_atomic.txt 10000 & ./5-3.out test_5-3_atomic.txt 10000
 * [1] 9885
 * アトミックな書き込みが完了しました
 * アトミックな書き込みが完了しました
 * [1]+  終了                     ./5-3.out test_5-3_atomic.txt 10000
 * $ ./5-3.out test_5-3_non-atomic.txt 10000 x & ./5-3.out test_5-3_non-atomic.txt 10000 x
 * [1] 9900
 * 非アトミックな書き込みが完了しました
 * 非アトミックな書き込みが完了しました
 * [1]+  終了                     ./5-3.out test_5-3_non-atomic.txt 10000 x
 * $ ls -l test_5-3_*
 * -rw-r--r--. 1 ka2601 ka2601 20000  6月 10 16:22 test_5-3_atomic.txt
 * -rw-r--r--. 1 ka2601 ka2601 20000  6月 10 16:22 test_5-3_non-atomic.txt
 *
 *  10000回の試行だと予想と違いアトミックでも非アトミックでも同じ結果になった。
 *  試行回数を100倍に増やしてもう一度実験した。
 *
 *  $ ./5-3.out test_5-3_atomic.txt 1000000 & ./5-3.out test_5-3_atomic.txt 1000000
 * [1] 10317
 * アトミックな書き込みが完了しました
 * アトミックな書き込みが完了しました
 * [1]+  終了                     ./5-3.out test_5-3_atomic.txt 1000000
 * $ ./5-3.out test_5-3_non-atomic.txt 1000000 x & ./5-3.out test_5-3_non-atomic.txt 1000000 x
 * [1] 10332
 * 非アトミックな書き込みが完了しました
 * 非アトミックな書き込みが完了しました
 * [1]+  終了                     ./5-3.out test_5-3_non-atomic.txt 1000000 x
 * 
 *  実行後のファイルを確認すると以下のようになった。
 *  $ ls -l test_5-3_*
 * -rw-r--r--. 1 ka2601 ka2601 2000000  6月 10 16:30 test_5-3_atomic.txt
 * -rw-r--r--. 1 ka2601 ka2601 1156666  6月 10 16:31 test_5-3_non-atomic.txt
 *
 *  100万回の試行だと非アトミックな書き込みを使ったとき複数プロセスがお互いの書いた内容を上書きしてしまうとわかった。
 *  (オープンファイルテーブルのファイルオフセットはプロセスごとに固有の領域を参照している(p102参照)ので、ファイルオフセットの
 *  更新タイミングによって(lseek()とwrite()の間に別プロセスがlseek()をした場合)各プロセスが同じところに書き込んでしまう)
 *  O_APPENDフラグを使えばファイルオフセットの移動と書き込みがアトミックに行われる(間に他のプロセスが入らない)ので確実に末尾に追記できる。
 */

