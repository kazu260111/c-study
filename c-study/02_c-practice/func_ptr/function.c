#include "func_ptr.h"
/* 関数の定義 */
int
add_calc(int a, int b) {
	return a + b;
}

int 
sub_calc(int a, int b) {
	return a - b;
}

int 
mul_calc(int a, int b) {
	return a * b;
}

int
menu_ui(enum MenuSelect status, int result) {
	switch (status) {
		case MENU_INIT:
			printf("簡易計算プログラムです。整数の四則演算(割り算を除く)が可能です。\n");
			break;
		case MENU_RESULT:
			printf("計算結果は%dです。\n", result);
			break;
		case MENU_QUIT:
			printf("[DEBUG] (未実装) qコマンドで終了するときのメッセージはここ\n");
			break;
		case MENU_ERROR:
			printf("[DEBUG] 今のところエラーが起きることはない\n");
			break;
		default:
			printf("[DEBUG] 実行されないはずのコードです\n");
			break;
	}
	return 0;
}

int 
system_ui(enum SystemMessage status) {
	switch (status) {
	case SYSTEM_INIT:
		printf("簡易計算プログラムを起動しています。\n");
		break;

	case SYSTEM_FINI:
		printf("簡易計算プログラムを終了します。\n");
		break;

	case SYSTEM_ERROR_NULL:
		printf("[DEBUG] 演算子がNULLのまま進行しようとしています。\n");
		break;
	default:
		printf("[DEBUG] これは実行されないはずのコードです\n");
		break;
	}
	return 0;
}

bool check_line_is_num(char *line, int *cmd) {
          if (sscanf(line, " %d", cmd) != 1) {
                  return false;
          }
          return true;
}

void
clear_input_buffer (void) {
          int c;
          while ((c = getchar()) != '\n' && c != EOF) {
          }
  return;
  }

bool
get_cmd(char *line, int size) {
        if (fgets(line, size, stdin) == NULL || line[0] == '\n') {
                return false;
        }
        /* 入力バッファに入力が残っていたら消す */
        if (strchr(line, '\n') == NULL) {
        clear_input_buffer();
        }
        return true;
}

int
num_select_ui(enum NumSelect status, struct Calc *calc) {
	switch (status) {
		case (NUM_FIRST):
			while (1) {
				printf("最初の数字を入力してください。\n");
				char line[256];
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。やり直してください。\n");
					continue;
				}
				int cmd = 0;
				if (check_line_is_num(line, &cmd) == false) {
					printf("数字が読み取れませんでした。やり直してください。\n");
					continue;
				}
				calc->first = cmd;
				break;
			}
		case (NUM_SECOND):
			while (1) {
				printf("二番目の数字を入力してください。\n");
				char line[256];
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。やり直してください。\n");
					continue;
				}
				int cmd = 0;
				if (check_line_is_num(line, &cmd) == false) {
					printf("数字が読み取れませんでした。やり直してください。\n");
					continue;
				}
				calc->second = cmd;
				break;
			}
			break;
		default:
			printf("[DEBUG] これは実行されないはずのコードです\n");
			break;
	}	
	return 0;
}

int
select_mani(struct Calc *calc) {
	while (1) {
		printf("演算子を選択してください。\n"
			"1) +\n"
			"2) -\n"
			"3) ×\n"
			);
		char line[256];
		if (get_cmd(line, sizeof(line)) == false) {
			printf("入力がありませんでした。やり直してください。\n");
			continue;
		}
		int cmd = 0;
		if (check_line_is_num(line, &cmd) == false) {
			printf("数字が読み取れませんでした。やり直してください。\n");
			continue;
		}
		switch (cmd) {
			case 1:
				calc->ope_func = add_calc;
				break;
			case 2:
				calc->ope_func = sub_calc;
				break;
			case 3:
				calc->ope_func = mul_calc;
				break;
			default:
				printf("[DEBUG] これは実行されないはずのコードです\n");
				break;
			}
	}
	return 0;
}

int
calc_execute(struct Calc *calc) {
	int a = calc->first;
	int b = calc->second;
	calc->first = calc->ope_func(a, b);
	calc->second = 0;
	calc->ope_func = NULL;
	return 0;
}

