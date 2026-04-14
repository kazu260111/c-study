#include "member.h"
#include <stdio.h>
#include <stdbool.h>
/* このファイルはユーザ向けのUIを担当する */

void start_message_ui() {
	printf("会員情報管理プログラムを起動します。\n");
	return;
}

void read_file_ui(enum ReadFileStatus status, int loaded_count) {
	if (status == READ_START) {
		printf("セーブファイルの読み込みを開始します。\n");
	}
	else if (status == READ_SUCCESS) {
		printf("セーブファイルの読み込みが完了しました。\n"
			"全部で%d件のデータが見つかりました。\n", loaded_count);
	}
	else if (status == READ_FAIL) {
		printf("セーブファイルが見つかりませんでした。"
			"初回の新規会員登録時に自動的にセーブファイルが作成されます。\n");
	}
	else if (status == READ_EMPTY_FILE) {
		printf("空のセーブファイルが見つかりました。");
	}
	return;
}

enum MenuSelectCmd menu_ui() {
	while (1) {
		printf("\n");
		printf("どの操作をしますか？\n");
		printf("1) 登録\n"
			"2) 編集\n"
		       	"3) 削除\n"
			"4) 閲覧\n"
			"q) やめる\n"
		      );
		char line[128];		
		if (get_cmd(line, sizeof(line)) == false) {
			printf("入力が読み取れませんでした。\n");
			}
		char cmd_line[128];
		check_line_is_str(line, cmd_line);
		if (cmd_line[0] == 'q' || cmd_line[0] == 'Q') {
			printf("プログラムを終了します。\n")
			return SELECT_QUIT;
			}
		int cmd;	
		if (check_line_is_num(line, &cmd) == false) {
			printf("数字が読み取れませんでした。やり直してください。\n");
			continue;
			}
		if (cmd == 1) {
			return SELECT_REGISTER;
		}
		else if (cmd == 2) {
			return SELECT_EDIT;
		}
		else if (cmd == 3) {
			return SELECT_DELETE;
		}
		else if (cmd == 4) {
			return SELECT_VIEW;
		}
		else {
			printf("数字が間違っています。やり直してください。\n");
			continue;
		}
	}
	return -1;
}

bool display_member(struct Member *head) {
	if (head == NULL) {
		return false;
	}
		
	struct Member *current_address = head;
	struct Member *next_address = head->next;
	/* 最上段に項目を表示 */
	printf("会員番号 名前 クラス 年齢 性別 備考\n"); 
	/* 最初の一人を表示
	printf("%d %s %d %d %s %s\n", current_address->member_num, current_address->name, current_address->class_num, current_address->age, current_address->gender, current_address->note);
	while (next_address != NULL) {
		current_address = current_address->next;
		next_address = next_address->next;
		printf("%d %s %d %d %s %s\n", current_address->member_num, current_address->name, current_address->class_num, current_address->age, current_address->gender, current_address->note);
	}
	return true;	
}

bool display_solo_member(struct Member *temp);

enum CancelCheck register_ui(enum RegisterStatus status, struct Member *temp);

enum CancelCheck edit_ui(enum EditStatus status, struct Member *temp);

enum DeleteStatus delete_ui(enum DeleteStatus status, struct Member *temp);  

void view_ui(enum ViewStatus status, struct Member *head);  

void quit_message_ui();  

void write_file_ui(struct Member *head);

