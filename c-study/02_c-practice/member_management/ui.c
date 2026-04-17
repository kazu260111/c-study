#include "member.h"

/* このファイルはユーザ向けのUIを担当する */

void start_message_ui() {
	printf("会員情報管理プログラムを起動します。\n");
	return;
}

void read_file_ui(enum ReadFileStatus status, int loaded_count) {
	switch (status) {
		case READ_START:
			printf("セーブファイルの読み込みを開始します。\n");
			return;
		case READ_SUCCESSED:
			printf("セーブファイルの読み込みが完了しました。\n"
			"全部で%d件のデータが見つかりました。\n", loaded_count);
			return;
		case READ_FAILED:
			printf("セーブファイルが見つかりませんでした。\n"
				"初回の新規会員登録時に自動的にセーブファイルが作成されます。\n");
			return;
		case READ_EMPTY_FILE:
			printf("空のセーブファイルが見つかりました。\n");
			return;
		case READ_ERROR_MEMORY:
			printf("メモリエラーが発生しました。データ消失を回避するためにプログラムを終了します。\n");
			exit(1);
		case READ_ERROR_ABNORMAL_DATA:
			printf("異常なデータが検出されました。プログラムを終了します。\n");
			exit(1);
		case READ_MAX_MEMBER:
			printf("読み込んだデータ件数が最大に達しました。読み込みを終了します。\n");
			return;
		default:
			assert(0 && "[DEBUG] read_file_uiの戻り値で問題が発生しました。");
	}
	assert(0 && "[DEBUG] read_file_uiで問題が発生しました。");
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
			printf("プログラムを終了します。\n");
			return SELECT_QUIT;
			}
		int cmd;	
		if (check_line_is_num(line, &cmd) == false) {
			printf("数字が読み取れませんでした。やり直してください。\n");
			continue;
			}
		switch (cmd) {
			case 1:
				return SELECT_REGISTER;
			case 2: 
				return SELECT_EDIT;
			case 3:
				return SELECT_DELETE;
			case 4:
				return SELECT_VIEW;
			default:
				printf("数字が間違っています。やり直してください。\n");
		}
	}
	assert(0 && "[DEBUG] menu_uiのループで問題が発生しました。");
}

enum SearchMemberStatus display_member(struct Member *head) {
	if (head == NULL) {
		return NOT_FOUND;
	}
	/* これまで画面に表示した会員数 */
	int display_count = 0;
	struct Member *current_address = head;
	struct Member *next_address = head->next;
	/* 最上段に項目を表示 */
	printf("\n会員番号 名前 クラス 年齢 性別 備考\n"); 
	/* 最初の一人を表示 */
	if (current_address->is_deleted_account == ACCOUNT_IS_ACTIVE) {
		printf("%d %s %d %d %s %s\n", current_address->member_num, current_address->name, current_address->class_num, current_address->age, current_address->gender, current_address->note);
		++display_count; 
	}
	/* ループで全員表示 */
	while (next_address != NULL) {
		current_address = current_address->next;
		next_address = next_address->next;
		if (current_address->is_deleted_account == ACCOUNT_IS_ACTIVE) {
			printf("%d %s %d %d %s %s\n",
				current_address->member_num,
			       	current_address->name,
			       	current_address->class_num, 
				current_address->age, 
				current_address->gender, 
				current_address->note
				);
			++display_count;
		}
	}
	/* 画面に一人も表示されなかったら */
	if (display_count == 0) {
		return NOT_FOUND;
	}
	return FOUND;	
}

enum SearchMemberStatus display_solo_member(struct Member *temp) {
		if (temp->is_deleted_account == ACCOUNT_IS_ACTIVE) {
			printf("\n%d %s %d %d %s %s\n",
				temp->member_num,
			       	temp->name,
			       	temp->class_num, 
				temp->age, 
				temp->gender, 
				temp->note
				);
			return FOUND;
		}
		else {
			return IS_DELETED;
		}
}
	

enum CancelCheck register_ui(enum RegisterStatus status, struct Member *temp) {
	switch (status) {
		case REGISTER_START:
			printf("新しい会員を登録します。必要な情報を確認してください。未入力でEnterを押すと登録を中断します。\n");
			printf("必要な情報：1.名前 2.クラス 3.年齢 4.性別 5.備考\n\n");	
			return CONTINUE;
		case REGISTER_NAME:
			while (1) {
				printf("名前を入力してください。\n");
				char line[1024] = {0};
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。登録を中断します。\n");
					return CANCEL;
				}
				if (check_line_is_str(line, temp->name) == false) {
					printf("文字が読み取れませんでした。やり直してください。\n");
					continue;
				}
				printf("名前：%s\n\n", temp->name);
				return CONTINUE;
			}
		case REGISTER_CLASS:
			while (1) {
				printf("半角数字でクラスを入力してください。(1級~%d級)\n", MAX_CLASS);
				char line[128];
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。登録を中断します。\n");
					return CANCEL;
				}
				if (check_line_is_num(line, &temp->class_num) == false) {
					printf("数字が読み取れませんでした。やり直してください。\n");
					continue;
				}
				if (temp->class_num < 1 || temp->class_num > MAX_CLASS) {
					printf("クラスは1級~%d級までの範囲に設定してください。\n", MAX_CLASS);
					continue;
				}
				printf("クラス：%d級\n\n", temp->class_num);
				return CONTINUE;
			}
		case REGISTER_AGE:
			while (1) {
				printf("半角数字で年齢を入力してください。\n");
				char line[128];
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。登録を中断します。\n");
					return CANCEL;
				}
				if (check_line_is_num(line, &temp->age) == false) {
					printf("数字が読み取れませんでした。やり直してください。\n");
					continue;
				}
				printf("年齢：%d\n\n", temp->age);
				return CONTINUE;
			}
		case REGISTER_GENDER:
			while (1) {
				printf("半角数字で性別を選択してください。\n"
					"1) 男性\n"
					"2) 女性\n"
					"3) その他\n"
				      );
				char line[256];
				int cmd;
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。登録を中断します。\n");
					return CANCEL;
				}
				if (check_line_is_num(line, &cmd) == false) {
					printf("数字が読み取れませんでした。やり直してください。\n");
					continue;
				}
				switch (cmd) {
					case 1:
						strcpy(temp->gender, "男性");
						break;
					case 2:
						strcpy(temp->gender, "女性");
						break;
					case 3:
						strcpy(temp->gender, "その他");
						break;
					default:
						printf("数字が間違っています。やり直してください。\n");
						continue;
				}
				printf("性別：%s\n\n", temp->gender);
				return CONTINUE;
			}
		case REGISTER_NOTE:
			printf("備考を入力してください。未入力でも次に進みます。\n");	
			char line[4096];
			if (get_cmd(line, sizeof(line)) == false) {
				strcpy(temp->note, "なし");
				printf("備考：%s\n\n", temp->note);
				return CONTINUE;
				}
			if (check_line_is_str(line, temp->note) == false) {
				strcpy(temp->note, "なし");
				printf("備考：%s\n\n", temp->note);
				return CONTINUE;	
				}
			printf("備考：%s\n\n", temp->note);
			return CONTINUE;
		case REGISTER_CONFIRM:
			while (1) {
				printf(	"会員番号：%d\n"
				"名前：%s\n"
				"クラス：%d\n"
				"年齢：%d\n"
				"性別：%s\n"
				"備考：%s\n"
				"以上の内容で会員情報を登録します。よろしいですか？[y/n]\n",
				temp->member_num,
				temp->name,
				temp->class_num,
				temp->age,
				temp->gender,
				temp->note
			      );
				char line[128] = {0};
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。やり直してください。\n");
					continue;
				}
				char cmd_line[128];
				if (check_line_is_str(line, cmd_line) == false) {
					printf("入力が読み取れませんでした。やり直してください。\n");
					continue;
				}
				enum YesNoRetry cmd = check_yes_or_no(cmd_line);
				switch (cmd) {
					case YES:
						printf("登録処理を実行します。\n");
						return CONTINUE;
					case NO:
						printf("登録をキャンセルします。\n");
						return CANCEL;
					case RETRY:
						printf("入力が間違っています。やり直してください。\n");
						continue;
				}
			}
		case REGISTER_COMPLETED:
			printf("登録が完了しました。ファイルへの書き込みを実行します。\n");
			return CONTINUE;
		case REGISTER_ERROR_MAX_MEMBER:
			printf("登録できません。会員数が最大登録可能数の上限です。メニューに戻ります。\n");
			return CANCEL;
		case REGISTER_ERROR_UNKNOWN:
			printf("不明なエラーが発生しました。メニューに戻ります。\n");
			return CANCEL;
		case REGISTER_ERROR_MEMORY:
			printf("メモリエラーが発生しました。メニューに戻ります。\n");
			return CANCEL;
		default:
			assert(0 && "[DEBUG] register_uiの引数で問題が発生しました。");
	}
	assert(0 && "[DEBUG] register_uiのループで問題が発生しました。");
}

enum CancelCheck edit_ui(enum EditStatus status, struct Member *temp) {
	switch (status) {
		case EDIT_START:
			printf("入会中の会員情報の編集をします。\n");
			return CONTINUE;
		case EDIT_CANCEL:
			printf("会員情報の編集を中止します。\n");
			return CANCEL;
		case EDIT_MEMBER_NOT_FOUND:
			printf("指定の会員が見つかりません。メニューに戻ります。\n");
			return CANCEL;
		case EDIT_NO_MEMBER:
			printf("編集できる会員がいません。メニューに戻ります。\n");
			return CANCEL;
		case EDIT_SELECT_MEMBER:
			while (1) {
				printf("編集したい会員の会員番号を入力してください。\n");
				char line[128];
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。会員情報の編集を中止します。\n");
					return CANCEL;
				}
				if (check_line_is_num(line, &temp->member_num) == false) {
					printf("数字を読み取れませんでした。やり直してください。\n");
					continue;
				}
				return CONTINUE;
			}
			return CONTINUE;
		case EDIT_MEMBER_IS_DELETED:
			printf("選択した会員はすでに退会済みです。メニューに戻ります。\n");
			return CANCEL;
		case EDIT_ITEM:
			while (1) {
				display_solo_member(temp);
				printf(	"1) 名前  "
					"2) クラス "
					"3) 年齢  "
					"4) 性別  "
					"5) 備考\n"
				      );
				printf("どの項目を編集しますか？\n");
				char line[128];
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。会員情報の編集を中止します。\n");
					return CANCEL;
				}
				int item_num;
				if (check_line_is_num(line, &item_num) == false) {
					printf("数字を読み取れませんでした。やり直してください。\n");
					continue;
				}
				switch (item_num) {
					case 1:
						register_ui(REGISTER_NAME, temp);
						break;
					case 2:
						register_ui(REGISTER_CLASS, temp);
						break;
					case 3:
						register_ui(REGISTER_AGE, temp);
						break;
					case 4:
						register_ui(REGISTER_GENDER, temp);
						break;
					case 5:
						register_ui(REGISTER_NOTE, temp);
						break;
					default:
						printf("数字が間違っています。やり直してください。\n");
						continue;
				}


			       	while (1) {
					display_solo_member(temp);
					printf("続けて編集しますか？[y/n]\n");
					char line[128] = {0};
					if (get_cmd(line, sizeof(line)) == false) {
						printf("入力がありませんでした。やり直してください。\n");
						continue;
					}
					char cmd_line[128] = {0};
					if (check_line_is_str(line, cmd_line) == false) {
						 printf("入力が読み取れませんでした。やり直してください。\n");
				    		continue;
							}
					switch (check_yes_or_no(cmd_line)) {
						case YES:
							break;
						case NO:
							return CONTINUE;
						case RETRY:
							printf("間違った入力です。やり直してください。\n");
							continue;
					}
					break;
				}
				continue;
			}
			assert(0 && "[DEBUG] edit_uiのEDIT_ITEMのループで問題が発生しました。");
		case EDIT_CONFIRM:
			while (1) {
				printf(	"会員番号：%d\n"
				"名前：%s\n"
				"クラス：%d\n"
				"年齢：%d\n"
				"性別：%s\n"
				"備考：%s\n"
				"以上の内容で編集を確定します。よろしいですか？[y/n]\n",
				temp->member_num,
				temp->name,
				temp->class_num,
				temp->age,
				temp->gender,
				temp->note
			      );
				char line[128] = {0};
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。やり直してください。\n");
					continue;
				}
				char cmd_line[128];
				if (check_line_is_str(line, cmd_line) == false) {
					printf("入力が読み取れませんでした。やり直してください。\n");
					continue;
				}
				enum YesNoRetry cmd = check_yes_or_no(cmd_line);
				switch (cmd) {
					case YES:
						printf("編集処理を実行します。\n");
						return CONTINUE;
					case NO:
						printf("編集をキャンセルします。\n");
						return CANCEL;
					case RETRY:
						printf("入力が間違っています。やり直してください。\n");
						continue;
				}
			}
		case EDIT_COMPLETED:
			printf("編集が完了しました。ファイルへの書き込みを行います。\n");
			return CONTINUE;
		default:
			assert(0 && "[DEBUG] edit_uiの引数で問題が発生しました。");
	}
	assert(0 && "[DEBUG] edit_uiで問題が発生しました。");
}

enum CancelCheck delete_ui(enum DeleteStatus status, struct Member *temp) {
	switch (status) {
		case DELETE_START:
			printf("会員情報の削除をします。\n");
			return CONTINUE;
		case DELETE_CANCEL:
			printf("会員情報の削除を中止します。\n");
			return CANCEL;
		case DELETE_MEMBER_NOT_FOUND:
			printf("指定した会員が見つかりません。メニューに戻ります。\n");
			return CANCEL;
		case DELETE_NO_MEMBER:
			printf("削除できる会員が見つかりません。メニューに戻ります。\n");
			return CANCEL;
		case DELETE_SELECT_MEMBER:
			while (1) {
				printf("削除したい会員の会員番号を入力してください。\n");
				char line[128];
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。会員情報の削除を中止します。\n");
					return CANCEL;
				}
				if (check_line_is_num(line, &temp->member_num) == false) {
					printf("数字を読み取れませんでした。やり直してください。\n");
					continue;
				}
				return CONTINUE;
			}
			return CONTINUE;
		case DELETE_CONFIRM:
			while (1) {
				display_solo_member(temp);
				printf("以上の会員を削除します。よろしいですか？[y/n]\n");
				char line[128] = {0};
				if (get_cmd(line, sizeof(line)) == false) {
					printf("入力がありませんでした。やり直してください。\n");
					continue;
				}
				char cmd_line[128];
				if (check_line_is_str(line, cmd_line) == false) {
					printf("入力が読み取れませんでした。やり直してください。\n");
					continue;
				}
				enum YesNoRetry cmd = check_yes_or_no(cmd_line);
				switch (cmd) {
					case YES:
						printf("会員削除処理を実行します。\n");
						return CONTINUE;
					case NO:
						printf("会員削除をキャンセルします。\n");
						return CANCEL;
					case RETRY:
						printf("入力が間違っています。やり直してください。\n\n");
						continue;
				}
			}
		case DELETE_COMPLETED:
			printf("会員情報の削除が完了しました。ファイルへの書き込みを行います。\n");
			return CONTINUE;
		case DELETE_IS_ALREADY_DELETED:
			printf("すでに退会済みの会員です。メニューに戻ります。\n");
			return CANCEL;
		default:
			assert(0 && "[DEBUG] deltete_uiの引数で問題が発生しました。");
	}
	assert(0 && "[DEBUG] delete_uiで問題が発生しました。");
}

void view_ui(enum ViewStatus status) {
	switch (status) {
		case VIEW_START:
			printf("入会中の会員情報を閲覧します。\n");
			return;
		case VIEW_WAIT:
			printf("終了する場合Enterキーを押してください。\n");
			char line[128];
			get_cmd(line, sizeof(line));
			return;
		case VIEW_EXIT:
			printf("会員情報の閲覧を終了します。\n");
			return;
		case VIEW_NO_MEMBER:
			printf("閲覧できる会員がいません。メニューに戻ります。\n");
			return;
		default:
			assert(0 && "[DEBUG] view_uiの引数で問題が発生しました。");
	}
	assert(0 && "[DEBUG] view_uiで問題が発生しました。");
}


void quit_message_ui() {
      printf("会員情報管理システムを終了します。\n");
      return;
}

void write_file_ui(enum WriteFileStatus status) {
	switch (status) {
		case WRITE_START:
			printf("ファイルへの書き込みを開始します。\n");
			break;
		case WRITE_FAILED:
		        printf("ファイルへの書き込みが失敗しました。メニューに戻ります。\n");
			break;
		case WRITE_COMPLETED:
			printf("ファイルへの書き込みが完了しました。メニューに戻ります。\n");
			break;
		default:
			assert(0 && "[DEBUG] write_file_uiの引数で問題が発生しました。");
	}
	return;
}

