/*
 * 5-7 readv(), write(v)の実装
 * read()やwrite()を一回だけにすることでアトミックな処理にする
 */
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/uio.h>

ssize_t my_readv(int fd, const struct iovec *iov, int iovcnt) {
	/* 読み取る総バイト数を計算 */
	ssize_t sum_read = 0;
	for (int count = 0; count < iovcnt; ++count) {
		sum_read += iov[count].iov_len;
	}
	/* 読み取るバイト数だけmalloc()でメモリ確保 */
	char *buf = malloc(sum_read);
	if (!buf) {
		fprintf(stderr, "malloc()失敗: %s\n", strerror(errno));
		return -1;
	}
	/* read()の実行 */
	ssize_t n_read = read(fd, buf, sum_read);
	if (n_read == -1) {
		fprintf(stderr, "read()失敗: %s\n", strerror(errno));
		free(buf);
		return -1;
	}
	
	/* read()してbufにあるデータをiov.iov_baseに格納 */
	char *ptr = buf;
	ssize_t rest_count = n_read;  /* 実際に読めたバイト数 */
	for (int count = 0; count < iovcnt; ++count) {
		/* rest_countが足りないなら残りをすべて読む */
		if (rest_count <= (ssize_t)iov[count].iov_len) {
			memcpy(iov[count].iov_base, ptr, rest_count);
		       	break;
		}
		memcpy(iov[count].iov_base, ptr, iov[count].iov_len);
		rest_count -= (ssize_t)iov[count].iov_len;
		ptr += iov[count].iov_len;
	}
	free(buf);
	return n_read;
}

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt) {
	/* 用意するバッファのサイズを調べる */
	ssize_t sum_write = 0;
	for (int count = 0; count < iovcnt; ++count) {
		sum_write += iov[count].iov_len;
	}
	char *buf = malloc(sum_write);
	if (!buf) {
		fprintf(stderr, "malloc()失敗: %s\n", strerror(errno));
		return -1;
	}
	/* 書き込むためのバッファにデータをコピーしていく */
	char *ptr = buf;
	for (int count = 0; count < iovcnt; ++count) {
		memcpy(ptr, iov[count].iov_base, iov[count].iov_len);
		ptr += iov[count].iov_len;
	}
	ssize_t n_write = write(fd, buf, sum_write);
	if (n_write == -1) {
		fprintf(stderr, "書き込みに失敗しました: %s\n", strerror(errno));
		free(buf);
		return -1;
	}
	free(buf);
	return n_write;
}

int main(void) {
	printf("open()でファイル\"test.txt\"を開きます\n");
	int fd = open("test.txt", O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open()に失敗しました: %s\n", strerror(errno));
		return 1;
	}
	printf("my_readv()でファイルを読み取ります。\n");
	char buf1[8] = {0};
	char buf2[16] = {0};
	char buf3[8] = {0};
	
	struct iovec iov[3] = {
		{buf1, sizeof(buf1)},
		{buf2, sizeof(buf2)},
		{buf3, sizeof(buf3)}
	};

	if (my_readv(fd, iov, 3) == -1) {
		fprintf(stderr, "my_readv()失敗\n");
		close(fd);
		return 1;
	}

	printf("my_writev()で新しいファイル\"out.txt\"に読んだ文字列を書き込みます\n");
	int fd2 = open("out.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd2 == -1) {
		fprintf(stderr, "open()に失敗しました: %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	
	/* my_writev()で読み取れた文字が少ない場合でも32byte書き込む仕様になっている */
	if (my_writev(fd2, iov, 3) == -1) {
		fprintf(stderr, "my_writev()失敗\n");
		close(fd);
		close(fd2);
		return 1;
	}
	printf("書き込みが完了しました\n");
	close(fd);
	close(fd2);
	return 0;
}	
