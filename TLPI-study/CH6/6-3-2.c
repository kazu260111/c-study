/* 6-3 unsetenv()で重複した環境変数の削除をテスト */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

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

int main(void) {
	extern char **environ;
	/* NULLを忘れないよう注意 */
	char *dup_test[] = {
		"A=1",
		"B=2",
		"C=3",
		"A=50",
		"A=10",
		NULL
	};
	environ = dup_test;
	my_unsetenv("A");
	if (getenv("A")) {
		printf("環境変数Aが存在します\n");
		return 1;
	}
	printf("重複して存在していた環境変数Aがすべて消去されました\n");
	return 0;
}

