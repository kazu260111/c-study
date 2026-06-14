/*
 * 5-4 dup2()の実装
 * 既存のファイルと使いたいファイルディスクリプタを指定して既存ファイルを開いた後dup2()を実行する。
 * 指定したファイルディスクリプタで開けているか確認した後、オープンファイルテーブルの共有を確認する
 * 変更可能なフラグは p99を参考にした。
 */

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	/* 引数のチェック */
	if (argc <= 2 || argc >= 4) {
		fprintf(stderr, "使い方: %s [ファイル名] [新しく使いたいファイルディスクリプタ番号]\n", argv[0]);
		return 1;
	}
	/* strtol()の処理 */
	char *endptr = 0;
	errno = 0;
	long val = strtol(argv[2], &endptr, 10);
	if (errno == ERANGE) {
		fprintf(stderr, "longの値の範囲外です: %s\n", strerror(errno));
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
		fprintf(stderr, "ファイルディスクリプタは正の数にしてください\n");
		return 1;
	}
	/* 最初のファイル(dup2()をされるファイル)を開く */
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open()失敗: %s\n", strerror(errno));
		return 1;
	}
	int fd_dup2 = (int)val;
	/* 指定したファイルディスクリプタが無効かどうか確認する */
	/* F_GETFLが失敗したならファイルディスクリプタは無効のはず */
	if (fcntl(fd_dup2, F_GETFL) != -1) {
		/* もし指定したファイルディスクリプタがdup2()を実行されるファイルのファイルディスクリプタと同じなら何もせず終了 */
		if (fd == fd_dup2) {
			printf("指定したファイルディスクリプタですでに指定ファイルを開いています。\n");
			close(fd);
			return 0;
		}
		printf("ファイルディスクリプタ%dは使用されているので閉じます\n", fd_dup2);
		close(fd_dup2);
	}
	/* dup2()の実行 */
	fd_dup2 = fcntl(fd, F_DUPFD, fd_dup2);
	if (fd_dup2 == -1) {
		fprintf(stderr, "fcntl()失敗: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	printf("fd_dup2の作成が完了しました。\n");
	printf("read()でファイルを読み込んでオープンファイルテーブル(ファイルオフセット)が共有されているか確認します。\n");
	char buf[8] = {0};
	/* 7byte読み出す */
	ssize_t n_read = read(fd, buf, sizeof(buf) - 1);
	if (n_read == -1) {
		fprintf(stderr, "fdからのread()失敗: %s\n", strerror(errno));
		close(fd);
		close(fd_dup2);
		return 1;
	}
	printf("fdから%.*sを読み出しました(%zdバイト)\n", (int)n_read, buf, n_read);
	n_read = read(fd_dup2, buf, sizeof(buf) - 1);
	if (n_read == -1) {
		fprintf(stderr, "fd_dup2からのread()失敗: %s\n", strerror(errno));
		close(fd);
		close(fd_dup2);
		return 1;
	}
	printf("fd_dup2から%.*sを読み出しました(%zdバイト)\n", (int)n_read, buf, n_read);

	/* オープンファイルステータスフラグが共有されているかチェック */
	printf("fcntl()でオープンファイルステータスフラグが共有されているかチェックします\n");
	int flag = fcntl(fd, F_GETFL);
	if (flag == -1) {
		fprintf(stderr, "flagのfcntl()失敗: %s\n", strerror(errno));
		close(fd);
		close(fd_dup2);
		return 1;
	}
	int flag_dup2 = fcntl(fd_dup2, F_GETFL);
	if (flag_dup2 == -1) {
		fprintf(stderr, "flag_dup2のfcntl()失敗: %s\n", strerror(errno));
		close(fd);
		close(fd_dup2);
		return 1;
	}
	if (flag != flag_dup2) {
		fprintf(stderr, "オープンファイルステータスフラグが一致しません\n");
		close(fd);
		close(fd_dup2);
		return 1;
	}
	printf("現在オープンファイルステータスフラグは一致しています。\n");
	printf("fdのO_APPENDのフラグを有効にします。\n");
	flag |= O_APPEND;
	if (fcntl(fd, F_SETFL, flag) == -1) {
		fprintf(stderr, "fcntl()に失敗しました: %s\n", strerror(errno));
		close(fd);
		close(fd_dup2);
		return 1;
	}
	printf("次にfdとfd_dup2のフラグを比較します。\n");
	/* flag_dup2の更新 */
	flag_dup2 = fcntl(fd_dup2, F_GETFL);
	if (flag != flag_dup2) {
		fprintf(stderr, "fdとfd_dup2のフラグが一致しません。\n");
		close(fd);
		close(fd_dup2);
		return 1;
	}
	printf("fdとfd_dup2のフラグが一致しました。\n");
	printf("これでオープンファイルテーブルが一致していると確認できました。\n");
	printf("終了処理をします。\n");
	close(fd);
	close(fd_dup2);
	return 0;
}
/* 
 * 実行結果
 * $ ./5-4_dup2.out test.txt 5
 * fd_dup2の作成が完了しました。
 * read()でファイルを読み込んでオープンファイルテーブル(ファイルオフセット)が共有されているか確認します。
 * fdから1234567を読み出しました(7バイト)
 * fd_dup2から8901234を読み出しました(7バイト)
 * fcntl()でオープンファイルステータスフラグが共有されているかチェックします
 * 現在オープンファイルステータスフラグは一致しています。
 * fdのO_APPENDのフラグを有効にします。
 * 次にfdとfd_dup2のフラグを比較します。
 * fdとfd_dup2のフラグが一致しました。
 * これでオープンファイルテーブルが一致していると確認できました。
 * 終了処理をします。
 */
