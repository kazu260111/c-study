/* 
 * ファイルホールに対応したcpを実装する(p93)
 * 仕様:
 * readで一定のバイトを読んですべて0ならファイルホールとして処理する
 * (すべてのファイルホールをコピーすることはできない)
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#define BUF_SIZE 4096
#define IS_ALL_ZERO 1
#define IS_NOT_ALL_ZERO 0

/* ファイルホールコピー関数 */
int hole_copy(int dst_fd, off_t num_read) {
	/* lseekでnum_read分カーソルを進める */
	if (lseek(dst_fd, num_read, SEEK_CUR) == -1) {
		return -1;
	}
	return 0;
}

/* すべて0か判定する関数 */
int is_all_zero(ssize_t num_read, char *buf) {
	for (int i = 0; i < num_read; ++i) {
		if (buf[i] != '\0') {
			return IS_NOT_ALL_ZERO;
		}
	}
	return IS_ALL_ZERO;
}

int main(int argc, char *argv[]) {
	/*>>> 引数の処理 <<<*/
	/* 引数の数が正しいかチェック */
	if (argc != 3)	{
		fprintf(stderr, "使い方: %s [コピー元ファイル] [コピー先ファイル]\n", argv[0]);
		return 1;
	}
	/* コピー元・コピー先ファイルを開く */
	int src_fd = open(argv[1], O_RDONLY);
	if (src_fd == -1) {
		fprintf(stderr, "コピー元ファイルを開くのに失敗しました: %s\n", strerror(errno));
		return 1;
	}
	int dst_fd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (dst_fd == -1) {
		fprintf(stderr, "コピー先ファイルを開くのに失敗しました: %s\n", strerror(errno));
		close(src_fd);
		return 1;
	}
	/*>>> コピー処理 <<<*/
	ssize_t num_read;
	char buf[BUF_SIZE];
	/* readでコピー元から読めたらループ */
	while ((num_read = read(src_fd, buf, sizeof(buf))) > 0) {
		/* bufがすべて0かチェック */
		if (is_all_zero(num_read, buf) == IS_ALL_ZERO) {
			/* すべて0が確定したならlseekを使ってファイルホールコピー(カーソルを動かす) */
			if (hole_copy(dst_fd, num_read) == -1) {
				fprintf(stderr, "lseekに失敗しました: %s\n", strerror(errno));
				close(src_fd);
				close(dst_fd);
				return 1;
			}
			/* カーソルを動かしたのでcontinueで次のループに */
			continue;			
		}
		/* bufの途中で0以外があれば読み取った分だけコピー */
		ssize_t n = write(dst_fd, buf, num_read);
		if (n == -1) {
			fprintf(stderr, "コピー先のファイルへの書き込みに失敗しました: %s\n", strerror(errno));
			close(src_fd);
			close(dst_fd);
			return 1;
		}
	}
	/* whileループの条件式のreadでエラーが出るとこの処理が実行される */
	if (num_read == -1) {
		fprintf(stderr, "コピー元からのreadに失敗しました: %s\n", strerror(errno));
		close(src_fd);
		close(dst_fd);
		return 1;
	}
	/* 
	 * ループ後(読み取るものがなくなったあと)、末尾がファイルホールならその分を埋める
	 * (lseekでカーソルを動かすだけだと書き込まれずコピー先ファイルの論理サイズが小さくなってしまう)
	 */
	struct stat sb;
	/* 元のファイルサイズ情報を得る */
	if (fstat(src_fd, &sb) == -1) {
		fprintf(stderr, "fstatに失敗しました: %s\n", strerror(errno));
		close(src_fd);
		close(dst_fd);
		return 1;
	}
	/* コピー先ファイルを元のファイルサイズになるようファイルホールで埋める */
	if (ftruncate(dst_fd, sb.st_size) == -1) {
		fprintf(stderr, "ftruncateに失敗しました: %s\n", strerror(errno));
		close(src_fd);
		close(dst_fd);
		return 1;
	}

	/*>>> ファイルディスクリプタを閉じる <<<*/
	close(src_fd);
	close(dst_fd);
	return 0;
}
