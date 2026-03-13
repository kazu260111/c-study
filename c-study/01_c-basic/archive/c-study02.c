/************************************************************
 *                                                          *
 *　　　　　C実践プログラミング(オライリー)学習メモ         *
 *                                                          *
 ************************************************************/

#if 0

/* p55 文字列*/

#include <stdio.h>
#include <string.h>  /* strcpyを使うので*/
int main(void)
{
	char name[12];
	name[0] = 'S';  /* "を使うのは文字列の時で、使うと2バイトになりcharに入らない*/
	name[1] = 'A';
	name[2] = 'M';
	name[3] = '\0';
	printf("%s\n", name);

	char name2[12];
	strcpy(name2, "JOHN"); /* "を使うと自動的に\0を入れてくれる*/
	printf("%s\n", name2);

	return 0;
}

#endif


#if 0

/* p57 文字列の読み込み*/

#include <stdio.h>
#include <string.h>

int main(void)
{
	char line[100];
	printf("文字を入力してください\n");
	fgets(line, sizeof(line), stdin);  /*sizeofはコンパイル時に固定の数字に置き換えられる。"
	
	size_t len;  /* size_tは0以上の整数。メモリサイズや配列の要素数を扱う型。*/
	len = strlen(line);
	if ( len > 0 && line[len-1] == '\n') {
		line[len-1] = '\0';
	}	
		
	/* lenが0だとエラーになる可能性があるので、len > 0を条件に入れた。fgetsで標準入力を受けとるときにENTER(\n)も入ってしまうので、これを消したい。ヌル文字(\0)を入れて上書きする。 */

	printf("あなたが入力した文字は%sです。\n", line);
	return 0;
}
	

#endif





