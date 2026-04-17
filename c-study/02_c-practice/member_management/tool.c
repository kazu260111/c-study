#include "member.h"

/*===== tool.c =====*/
/*
 * ファイル説明:
 * よく使う便利な関数をまとめたファイル。
 */

bool get_cmd(char *line, int size) {
	if (fgets(line, size, stdin) == NULL || line[0] == '\n') {
		return false;
	}
	/* 入力バッファに入力が残っていたら消す */
	if (strchr(line, '\n') == NULL) {
	clear_input_buffer();
	}
	return true;
}

bool check_line_is_num(char *line, int *cmd) {
	if (sscanf(line, " %d", cmd) != 1) { 
		return false;
	}
	return true;
}

bool check_line_is_str(char *line, char *cmd_line) {
	if (sscanf(line, " %s", cmd_line) != 1) {
	       return false;
	}
	return true;
}

void clear_input_buffer (void) {
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {
	}
return;
}

enum YesNoRetry check_yes_or_no(char *cmd_line) {
	if (cmd_line[0] == 'y' || cmd_line[0] == 'Y') {
		return YES;
	}
	else if (cmd_line[0] == 'n' || cmd_line[0] == 'N') {
		return NO;
	}
	else {
		return RETRY;
	}
}



