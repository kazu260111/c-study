/* charの復習 */
#include <stdio.h>
#include <string.h>
int main(void){

char name[] = {'S', 'A', 'M', '\0'}; /*　一文字のときは'で囲む。\0で終端を表す */
char name2[] = "TOM";  /* この書き方でも良い、/0は不要 */
char name3[50];
strcpy(name3, "Mary");  /*一度配列を準備したあとstrcpyで文字を入れる。*/

printf("%s\n%s\n%s\n", name, name2, name3);

return 0;
}


