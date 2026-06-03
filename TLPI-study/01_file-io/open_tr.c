#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main(){
	int fd = open("text.txt", O_CREAT | O_RDWR | O_APPEND, 0644);
	if (fd == -1) {
		fprintf(stderr, "ファイルを開けませんでした %s\n", strerror(errno));
		return 1;
	}
	char text[] = "Alice has an apple.";
	ssize_t n = write(fd, text, strlen(text));
	if (n == -1) {
		fprintf(stderr, "書き込みに失敗しました。%s\n", strerror(errno));
		return 1;
	}
	printf("%zuバイトのテキストを書き込みました。\n", strlen(text));
	printf("%zdバイト書き込みました。\n", n);
	char buf[128];
	/* writeでファイルオフセットが進んでいるので0に戻してreadする */
	lseek(fd, 0, SEEK_SET);
	/* \0を入れるスペースを除いたバイト数を読み取る */
	n = read(fd, buf, sizeof(buf) - 1);
	if (n == -1) {
		fprintf(stderr, "読み取りに失敗しました %s\n", strerror(errno));
		return 1;
	}
	printf("%zdバイト読み取れました。\n", n);
	/* 読み取った末尾に\0を入れる(readはバイト列を直接読み取るので) */
	buf[n] = '\0';
	printf("読み取った内容: %s\n", buf);
	close(fd);
	return 0;
}
