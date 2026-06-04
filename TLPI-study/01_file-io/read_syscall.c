/* TLPI p69~ 参考 */
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main() {
	int fd = open("startup", O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "ファイルを開くのに失敗しました。:%s\n", strerror(errno));
		return 1;
	}
	/* O_CREATを使うときはmodeを第3引数に入れないと、スタックのゴミデータをパーミッション設定に使ってしまう */
	int fd_2 = open("file_1", O_RDWR | O_CREAT, 0644); 
	if (fd_2 == -1) {
		fprintf(stderr, "ファイルを開くのに失敗しました。 :%s\n", strerror(errno));
		close(fd);
		return 1;
	}
	printf("ファイルを開けるのに成功しました。\n");
	printf("fd=%d, fd_2=%d\n", fd, fd_2);
	close(fd);
	close(fd_2);

	return 0;	
}

