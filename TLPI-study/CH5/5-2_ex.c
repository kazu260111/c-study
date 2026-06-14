/* 5-2 O_APPENDを使って既存のファイルをオープンしてファイルの先頭へ移動し、データを書き込むプログラム */
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main() {
	/* 既存のファイルを開ける */
	int fd = open("test_5-2.txt", O_APPEND | O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "open()に失敗しました: %s\n", strerror(errno));
		return 1;
	}
	/* ファイルの先頭へ移動 */
	if (lseek(fd, 0, SEEK_SET) == -1) {
		fprintf(stderr, "lseek()失敗: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	char buf[128] = {'1', '2', '3', '4', '5'};
	/* 1~5と\0でファイル先頭から128byte埋める? */
	ssize_t n_write = write(fd, buf, sizeof(buf));
	if (n_write == -1) {
		fprintf(stderr, "write()失敗:%s\n", strerror(errno));
		close(fd);
		return 1;
	}
	if (n_write < (ssize_t)sizeof(buf)) {
		fprintf(stderr, "バッファが全て書き込めませんでした: %zdバイト書込み\n", n_write);
		close(fd);
		return 1;
	}
	/* 終了処理 */
	printf("書き込みが終了しました\n");
	close(fd);
	return 0;
}

/* 
 * 実行した結果、O_APPENDを有効にするとlseek()でファイル先頭に移動して書き込んでも、
 * 自動的にファイル末尾に移動して追記されることがわかった。
 * 
 * O_APPENDフラグはファイル末尾に移動->write()をアトミックに実行できるため確実にファイル末尾に追記できる
 * (他のプロセスが同じファイルを開けていても途中で割り込まれない)
 * p102にある通り、オープンファイルテーブルのファイルオフセットはプロセスごとに別で管理しているため、
 * アトミックでないとファイルオフセットを更新するタイミング次第で書き込む場所がおかしくなる場合がある。 
 */
