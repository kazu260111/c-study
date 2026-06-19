/* 6-3 getenv(), putenv()を使ってsetenv(), unsetenv()を実装する */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


int my_setenv(const char *name, const char *value, int overwrite) {
	/* overwriteが0ならgetenv()の値がNULLでないときreturn */
	if (!overwrite) {
		if (getenv(name)) {
			/* すでに環境変数が存在するのでreturn */
			return 0;
		}
	}
	/* putenv()に必要な"name=value\0"の分のメモリを確保する */
	size_t name_len = strlen(name);
	size_t value_len = strlen(value);
	char *p = malloc(name_len + value_len + 2);
	if (!p) {
		fprintf(stderr, "malloc()失敗: %s\n", strerror(errno));
		return -1;
	}
	/* 確保したメモリにセットするための文字列を格納 */
	memcpy(p, name, name_len);
	memcpy(p + name_len, "=", 1);  /* 普通に配列に代入してよい（この場合文字列リテラルのアドレスからコピー）*/
	memcpy(p + name_len + 1, value, value_len);
	memcpy(p + name_len + 1 + value_len, "\0", 1);  /* =と同じ('\0'とすると0番地から読もうとしてしまいクラッシュ) */
	
	/* putenv()で環境変数を設定 */
	/* putenv()は成功時に0、エラー時0以外を返す */
	if (putenv(p)) {
		free(p);
		return 1;
	}
	return 0;
}

int my_unsetenv(const char *name) {
	extern char **environ;
	char **ep = environ;
	size_t name_len = strlen(name);
	while (*ep) {
		if (strncmp(*ep, name, name_len) == 0 && (*ep)[name_len] == '=') {
			/* 目当ての環境変数が見つかったらenvironのリストをずらしていく */
			for (char **sp = ep; *sp != NULL; ++sp) {
				*sp = *(sp + 1);

			}
			/* ずらしたら現在指している場所をもう一度確認する(同一の環境変数が連続する場合に備える) */
			continue;
		}
		++ep;
	}
	return 0;
}

int main() {
	char name[] = {"MY_ENV_TEST"};
	/* 1回目のmy_setenv() */
	printf("上書きモードでmy_setenv()を実行します\n");
	if (my_setenv(name, "100", 1)) {
		fprintf(stderr, "my_setenv()失敗\n");
		return 1;
	}
	printf("my_setenv()が成功しました\n設定後: %s\n", getenv(name));
	/* 2回目のmy_setenv() (1回目の実行で確保したメモリがリークするが、仕様的に許容範囲となっている */
	printf("上書きモードでmy_setenv()を実行します\n");
	if (my_setenv(name, "200", 1)) {
		fprintf(stderr, "my_setenv()失敗\n");
		return 1;
	}
	/* 3回目のmy_setenv() (非上書きなので変更できずreturnし、成功扱い */
	printf("my_setenv()が成功しました\n設定後: %s\n", getenv(name));
	printf("非上書きモードでmy_setenv()を実行します\n");
	if (my_setenv(name, "500", 0)) {
		fprintf(stderr, "my_setenv()失敗\n");
		return 1;
	}
	printf("my_setenv()が成功しました\n設定後: %s\n", getenv(name));
	/* my_unsetenv()の実行 */
	printf("my_unsetenv()を実行します\n");
	if (my_unsetenv(name)) {
		fprintf(stderr, "my_unsetenv()失敗\n");
		return 1;
	}
	printf("my_unsetenv()が成功しました\n");
	if (!getenv(name)) {
		printf("環境変数%sは削除されました\n", name);
	}
	else {
		printf("環境変数%sが存在します\n", name);
		return 1;
	}
	printf("プログラムを終了します\n");
	return 0;
}
/*
 * 実行結果
 * $ ./6-3.out
 * 上書きモードでmy_setenv()を実行します
 * my_setenv()が成功しました
 * 設定後: 100
 * 上書きモードでmy_setenv()を実行します
 * my_setenv()が成功しました
 * 設定後: 200
 * 非上書きモードでmy_setenv()を実行します
 * my_setenv()が成功しました
 * 設定後: 200
 * my_unsetenv()を実行します
 * my_unsetenv()が成功しました
 * 環境変数MY_ENV_TESTは削除されました
 * プログラムを終了します
 */
