/*
 * 5-4 dup()の実装
 * 既存のファイルを指定して開いた後dup()を実行し、オープンファイルテーブルの共有を確認する
 */

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	/* 引数のチェック */
	if (argc <= 1 || argc >= 3) {
		fprintf(stderr, "使い方: %s [ファイル名]\n", argv[0]);
		return 1;
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open()失敗: %s\n", strerror(errno));
		return 1;
	}
	int fd_dup = fcntl(fd, F_DUPFD, 0);
	if (fd_dup == -1) {
		fprintf(stderr, "fcntl()失敗: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	printf("fd_dupの作成が完了しました。\n");
	printf("read()でファイルを読み込んでオープンファイルテーブル(ファイルオフセット)が共有されているか確認します。\n");
	char buf[8] = {0};
	/* 7byte読み出す(後でprintf()を使うので終端に\0を残しておく) */
	ssize_t n_read = read(fd, buf, sizeof(buf) - 1);
	if (n_read == -1) {
		fprintf(stderr, "fdからのread()失敗: %s\n", strerror(errno));
		close(fd);
		close(fd_dup);
		return 1;
	}
	printf("fdから%sを読み出しました(%zdバイト)\n", buf, n_read);
	n_read = read(fd_dup, buf, sizeof(buf) - 1);
	if (n_read == -1) {
		fprintf(stderr, "fd_dupからのread()失敗: %s\n", strerror(errno));
		close(fd);
		close(fd_dup);
		return 1;
	}
	printf("fd_dupから%sを読み出しました(%zdバイト)\n", buf, n_read);
	printf("ファイルを閉じてプログラムを終了します\n");
	close(fd);
	close(fd_dup);
	return 0;
}
/*
 * 実行結果
 * $ ./5-4_dup.out test.txt
 * fd_dupの作成が完了しました。
 * read()でファイルを読み込んでオープンファイルテーブル(ファイルオフセット)が共有されているか確認します。
 * fdから1234567を読み出しました(7バイト)
 * fd_dupから8901234を読み出しました(7バイト)
 * ファイルを閉じてプログラムを終了します
 */
/*
 * 改善案
 * 二回目のread()で7byte未満しか読めないと、二回目のprintf()のときに一回目のread()で読み取った
 * 文字が二回目のprintf()で表示されてしまう。対策としては%.*sを使うと読み取った文字の分(n_read)
 * だけ表示できるらしい。
 */
