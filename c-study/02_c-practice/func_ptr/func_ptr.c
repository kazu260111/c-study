#include "func_ptr.h"
/* 簡単な計算プログラム 
 * 関数ポインタの学習用に作成した
 */


/* main関数 */
int
main() {
	/* calcという構造体の変数をループしてからもずっと使いまわす */
	struct Calc calc = {0};
	system_ui(SYSTEM_INIT);
	menu_ui(MENU_INIT, 0);
	num_select_ui(NUM_FIRST, &calc);
		/* ループするならここから */
		select_mani(&calc);
		if (calc.ope_func == NULL) {
			system_ui(SYSTEM_ERROR_NULL);
			exit(1);
		}
		num_select_ui(NUM_SECOND, &calc);
		calc_execute(&calc);
		int result = calc.first;
		menu_ui(MENU_RESULT, result);
		/* 参考：calc.first以外を初期化しているのでループすれば計算結果を引き継ぎ計算できる */
	
	return 0;
}
