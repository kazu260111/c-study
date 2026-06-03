/* teeコマンドの実装(p93, 4-1) */

#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#define MAX_TEE_SIZE 4096

int main() {
	/* tee出力用のファイルを開く */
	int fd = open("tee_out.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "ファイルが開けませんでした: %s\n", strerror(errno));
		return 1;
	}
	char buf[MAX_TEE_SIZE];
	/* 標準入力を読み取る */
	ssize_t num_read;
	/* ループしてreadを繰り返す */
	while ((num_read = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
		/* 標準出力に書き出す */
		ssize_t n = write(STDOUT_FILENO, buf, num_read);  /* strlen(buf)とすると\0がなくて止まらなくなる */
		if (n == -1) {
			fprintf(stderr, "標準出力への書き出しが失敗しました: %s\n", strerror(errno));
			return 1;
		}
		/* tee_out.txtに内容を書き出す */
		n = write(fd, buf, num_read);
		if (n == -1) {
			fprintf(stderr, "ファイルへの書き込みが失敗しました: %s\n", strerror(errno));
			return 1;
		}
		num_read = read(STDIN_FILENO, buf, sizeof(buf));
	}
	/* 読み取りエラーならループを抜けてここに来る */
	if (num_read == -1) {
		fprintf(stderr, "読み取れませんでした: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}
	

