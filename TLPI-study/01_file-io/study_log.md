# 学習メモ
## ファイル入出力(1) 2026-05-27
### 学んだこと
- シェルは常に3つのファイルディスクリプタを開けている。
  - 標準入力(0)、標準出力(1)、標準エラー出力(2)
  - ただしfreopenなどで番号は変更される可能性がある
- 新しくopenすると、可能な限り小さい値(正の数)がファイルディスクリプタに割り当てられる
- openでは第2引数でファイルアクセスモードを指定する。ファイルを新規作成するときはmodeを第3引数に指定。
  - 複数のファイルアクセスモードを指定する場合は|を使う。
- closeのときはファイルディスクリプタを指定する。

### プログラム作成
- open,closeを使った簡単なプログラムを作成した。
```c
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
		close(fd);  /* エラーなら最初に開いたファイルを閉じる */
		return 1;
	}
	printf("ファイルを開けるのに成功しました。\n");
	printf("fd=%d, fd_2=%d\n", fd, fd_2);
	close(fd);
	close(fd_2);

	return 0;	
}

```
- 実行結果
```bash
$ ./read.out 
ファイルを開けるのに成功しました。
fd=3, fd_2=4  # 最も小さい値から割り当てられている
```
- さらにstraceコマンドでシステムコールを確認した。
```bash
$ strace -e trace=openat ./read.out  # glibcでopenはopenatに変換されるらしい
# ここからプログラム起動の準備
# 読み取り専用でexecしたらファイルディスクリプタを閉じる
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3  # キャッシュファイルを開く
openat(AT_FDCWD, "/lib64/libc.so.6", O_RDONLY|O_CLOEXEC) = 3  # 共有(動的)ライブラリを開く
# ここまでで使ったファイルディスクリプタはcloseで閉じられている
openat(AT_FDCWD, "startup", O_RDONLY)   = 3  # 使えるファイルディスクリプタで一番小さい数を使う
openat(AT_FDCWD, "file_1", O_RDWR|O_CREAT, 0644) = 4
ファイルを開けるのに成功しました。
fd=3, fd_2=4
+++ exited with 0 +++
```
- closeについてもシステムコールを調べてみる。
```bash
$ strace -e trace=openat,close ./read.out 
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
close(3)                                = 0  # 使ったあと閉じている
openat(AT_FDCWD, "/lib64/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
close(3)                                = 0  # 使ったあと閉じている
openat(AT_FDCWD, "startup", O_RDONLY)   = 3  # 起動準備のとき閉じていたので3番が使える中で一番小さい
openat(AT_FDCWD, "file_1", O_RDWR|O_CREAT, 0644) = 4  # 次に小さいのが4番
ファイルを開けるのに成功しました。
fd=3, fd_2=4
close(3)                                = 0
close(4)                                = 0
+++ exited with 0 +++
```

