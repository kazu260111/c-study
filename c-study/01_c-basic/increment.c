#include <stdio.h>
int main(void){

	/* 前置インクリメントの場合 */
	int i = 0;
	int j = 5;
	printf("i=%d, j=%d\n", i, j);
	i = ++j;
	printf("i=++jを実行すると、i=%d, j=%d\n", i, j);  /* i=6, j=6 */
		

	/* 後置インクリメントの場合 */
	i=0;  
	j=5;
	printf("i,jを初期化\n");
	printf("i=%d, j=%d\n", i, j);
	i = j++;
	printf("i=j++を実行すると、i=%d, j=%d\n", i, j);  /* i=5, j=6 */

	return 0;
}
	


	
