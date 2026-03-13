/* strの復習 */


#include <stdio.h>
#include <string.h>
int main(void){
	char name[30];
	strcpy(name, "SAM");
	printf("彼の名前は%sです。\n", name);
	
/* フルネームを完成させる練習   */

	char first[30];  /* 下の名前の配列 */
	char last[30];  /* 上の名前の配列 */
	strcpy(first, "TARO");  
	strcpy(last, "TANAKA");

	char full_name[100];
	strcpy(full_name, first);
       	strcat(full_name, " ");
	strcat(full_name, last);
	printf("彼のフルネームは%sです。\n", full_name);	
	
	return 0;
}

