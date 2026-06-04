/* ファイルホール付きファイルを作成するプログラム */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	/*>>> 引数の処理 <<<*/
	/* 引数の数が正しいかチェック */
	if (argc != 2) {
		fprintf(stderr, "使い方: %s [作成するファイル名]\n", argv[0]);
		return 1;
	}
	/*>>> ファイルの作成 <<<*/
	int fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "ファイルが作成できませんでした: %s\n", strerror(errno));
		return 1;
	}
	/*>>> ファイルへの書き込み <<<*/
	/* 最初の書き込み */
	char buf_1[] = "Alice was beginning to get very tired of sitting by her sister on the bank, and of having nothing to do:";
	ssize_t n = write(fd, buf_1, strlen(buf_1));
	if (n == -1) {
		fprintf(stderr, "書き込みに失敗しました: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	/* lseekでファイルホールの作成 */
	off_t offset = lseek(fd, 10000, SEEK_CUR);
	if (offset == -1) {
		fprintf(stderr, "lseekに失敗しました: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	/* 二回目の書き込み */
	char buf_2[] = "once or twice she had peeped into the book her sister was reading, but it had no pictures or conversations in it, “";
	n = write(fd, buf_2, strlen(buf_2));
	if (n == -1) {
		fprintf(stderr, "書き込みに失敗しました: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	/* lseekで二回目のファイルホールの作成 */
	offset = lseek(fd, 10000, SEEK_CUR);
	if (offset == -1) {
		fprintf(stderr, "lseekに失敗しました: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	/* 三回目の書き込み */
	char buf_3[] = "and what is the use of a book,” thought Alice “without pictures or conversations?”\n";
	n = write(fd, buf_3, strlen(buf_3));
	if (n == -1) {
		fprintf(stderr, "書き込みに失敗しました: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	/* lseekで三回目のファイルホールの作成 */
	offset = lseek(fd, 10000, SEEK_CUR);
	if (offset == -1) {
		fprintf(stderr, "lseekに失敗しました: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	/* このまま終わるとカーソルを動かしただけで最後のファイルホールが書き込めない */
	/* offsetにカーソルの絶対位置が入っているので、これを利用してftruncateで拡張する */
	if (ftruncate(fd, offset) == -1) {
		fprintf(stderr, "ftruncateに失敗しました: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	/*>>> 終了処理 <<<*/
	close(fd);
	return 0;
}
	

