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

	int fd;
	/* アトミックなファイルオープン処理 */
	if (argc == ATOMIC) {
		/* ファイルオープン */
		fd = open(argv[1], O_CREAT | O_APPEND | O_WRONLY, 0644);
		if (fd == -1) {
			fprintf(stderr, "open()失敗: %s\n", strerror(errno));
			return 1;
		}
	}
	/* 非アトミックなファイルオープン処理 */
	else if (argc == NON_ATOMIC) {
		/* ファイルオープン */
		fd = open(argv[1], O_CREAT | O_WRONLY, 0644);
		if (fd == -1) {
			fprintf(stderr, "open()失敗: %s\n", strerror(errno));
			return 1;
		}
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
	/* 終了処理 */
	close(fd);
	return 0;
}
