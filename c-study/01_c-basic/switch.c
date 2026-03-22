/* switchの練習 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	char line[20];
	char *p;  /* strtol用のポインタ */
	long l_num;
	char name[20];

	printf("選択してください\n1) りんご\n2) みかん\n3) もも\n4) いちご\n");
	fgets(line, sizeof(line), stdin);  /* lineの最後に\nが入ることに注意 */
	l_num = strtol(line, &p, 10);
	
	if ( *p == '\n' || *p == '\0' ) {  /* strtolが改行までを読み込めた場合（文字列を読まなかった場合） */
		switch (l_num) {
			case 1:
				printf("りんごが選択されました\n");
				break;
			case 2:
				printf("みかんが選択されました\n");
				break;
			case 3:
				printf("ももが選択されました\n");
				break;
			case 4:
				printf("いちごが選択されました\n");
				break;
			default:
				printf("不適切な入力です\n");
				break;
		}
		return 0;  /* ifで条件を満たしていたらここでプログラムを終了 */
	}

	/* 数字が読めなかったので文字列として読めるか試行 */
	sscanf(line, "%s", name);
	if (strcmp(name, "りんご") == 0) {  /* strcmpは文字列が完全一致したときに０を返す */
		printf("りんごが選択されました\n");
	}
	else if (strcmp(name, "みかん") == 0) {
		printf("みかんが選択されました\n");
	}
	else if (strcmp(name, "もも") == 0) {
		printf("ももが選択されました\n");
	}
	else if (strcmp(name, "いちご") == 0) {
		printf("いちごが選択されました\n");
	}
	else {
		printf("不適切な入力です\n");
	}
	return 0;
}
	
	
        	

