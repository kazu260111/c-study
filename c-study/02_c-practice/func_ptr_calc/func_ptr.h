#ifndef FUNC_PTR_H
#define FUNC_PTR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* enum型の定義 */
enum SystemMessage {
	SYSTEM_INIT = 0,
	SYSTEM_FINI = 1,
	SYSTEM_ERROR_NULL = 2
};

enum MenuSelect {
	MENU_INIT = 0,
	MENU_QUIT = 5,
	MENU_ERROR = 6,
	MENU_RESULT = 8
};

enum NumSelect {
	NUM_FIRST = 0,
	NUM_SECOND = 1,
	NUM_ERROR = 2
};

/* 構造体、関数、関数ポインタの定義、プロトタイプ宣言 */
typedef int
(*CalcPtr)(int, int);

struct Calc {
	int first;
	int second;
	CalcPtr ope_func;
};

int
menu_ui(enum MenuSelect status, int result);

int
system_ui(enum SystemMessage status);

int
select_mani(struct Calc *calc);

int
num_select_ui(enum NumSelect status, struct Calc *calc);

int
add_calc(int a, int b);

int 
sub_calc(int a, int b);

int 
mul_calc(int a, int b);

/*
 * 使い方: 渡された構造体のfirst,second部分をope_funcにある関数ポインタで計算して、結果をfirstに入れる。その後second, ope_funcは0に初期化される。
 * 引数: struct Calc *calc  - 計算用の構造体変数のポインタ 
 * 戻り値: 0  - 成功した場合
 *         1  - 失敗した場合
 */
int
calc_execute(struct Calc *calc);

bool
get_cmd(char *line, int size);

void
clear_input_buffer (void);

bool 
check_line_is_num(char *line, int *cmd);

#endif
