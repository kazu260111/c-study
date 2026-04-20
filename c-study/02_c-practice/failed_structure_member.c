/*
 * ===== 注意：このプログラムは動作しません。=====
 * 
 * 参考資料として残しています。動作するプログラムはmember_managementフォルダにあります。
 * 以下が動作するプログラムのURL
 * https://github.com/kazu260111/c-study/tree/main/c-study/02_c-practice/member_management
 *
 * このプログラムを書いたときのメモ（感想）はこのファイルの最後にあります。
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>

#define MAX_MEMBER 100  /*--- メンバーの最大人数を変えたい場合ここを変える ---*/

/*
 * === 登録した会員情報(スイミングスクール)を閲覧できるプログラム ===
 * 
 * 仕様：
 * 会員情報の登録・編集・削除・閲覧が可能。
 * 会員情報をファイルに保存して起動時に自動で読み込む。
 * 退会（削除）された会員はそのまま退会済みの会員としてファイルに残る。
 * 退会した会員は氏名を伏せ字にして備考を削除する。
 * 新規会員の会員番号はファイルに存在する最後尾の会員番号に+1して登録する。
 *
 * 登録情報：
 * 会員番号・名前・クラス（1~5級）・年齢・性別・備考 
 */


/*>>> 構造体の定義 <<<*/
struct Member {
	int member_num;  /* 会員番号 */
	char name[1024];  /* 氏名 */
	int class;  /* クラス（1~5級） */
	int age;  /* 年齢 */
     	int gender;  /* 性別　1:男性 2:女性 3:その他 */
	bool is_deleted_account;  /* 入会中なら0,退会済みなら1 */
	char note[4096];  /* 備考 */
	struct Member *next;  /* 次のメンバーのアドレス(プログラム起動ごとに毎回更新される */
};


/*>>> ファイルが空かチェックする関数 <<<*/
int  is_file_empty(FILE *fp) {
/*===== 編集中 =====*/
}

/*>>> ファイル読み出し関数 <<<*/
void load_func(struct Member *head) {
	FILE* fp = fopen("member_save.dat", "rb");
	
	/* もしファイルを読み取れなかったら */
	if (fp == NULL) {
		printf("セーブファイルが見つかりませんでした。新規セーブファイルを作成します。\n");
		fp = freopen("member_save.dat", "wb", fp);
		loaded_count = 0;  /* 読み出す会員数は0人で確定 */
		head = NULL /* 最初のメンバーのアドレスをNULLに設定 */
		fclose(fp);
		return;
	}
	
	/* 空のファイルかチェックする */
	if (is_file_empty(fp) == 1) {  /*=== ファイルが空かチェックする関数、1なら空ファイル ===*/
		printf("セーブファイルが空でした。\n");
		loaded_count = 0;  /* 読み出す会員数は0人で確定 */
		head = NULL  /* 最初のメンバーのアドレスをNULLに設定 */
		fclose(fp);
		return;
	}
	
	/* ファイルの読み出し */
	/*===== 編集中 =====*/	

}
/*>>> 会員情報登録時の未入力チェック関数 <<<*/
/* lineのアドレスとサイズを引数とする */

int regi_cancel_func(char *line, int size) {
	if (fgets(line, size, stdin) == NULL || line[0] == '\n') {
		printf("登録を中断します。\n");
		return -1;
	}
	return 0;
}	

/*>>> 会員情報編集時の未入力チェック関数 <<<*/
/* lineのアドレスとサイズを引数とする */

int edit_cancel_func(char *line, int size) {
	if (fgets(line, size, stdin) == NULL || line[0] == '\n') {
		printf("編集を中断します。\n");
		return -1;
	}
	return 0;
}	
/*>>> 会員情報登録時の数字読み取り用関数 <<<*/
/* lineのアドレスと数字のアドレスを引数とする */
int regi_read_num(char *line, int *out_num) {
	/* 数字が読み取れなかった場合 */
	if (sscanf(line, " %d", out_num) != 1) { 
		printf("入力が読み取れませんでした。やり直してください。\n");
		return -1;
	}
	return 0;
}

/*>>> 会員情報編集時の数字読み取り用関数 <<<*/
/* lineのアドレスと数字のアドレスを引数とする */
int edit_read_num(char *line, int *out_num) {
	/* 数字が読み取れなかった場合 */
	if (sscanf(line, " %d", out_num) != 1) { 
		printf("入力が読み取れませんでした。メニューに戻ります。\n");
		return -1;
	}
	return 0;
}

/*>>> 会員情報登録・編集時の名前読み取り用関数 <<<*/
int select_name_func (char *name) {
	while (1) {
		printf("1.名前を入力してください。\n");	
		
		/* 名前の入力がない場合 */
		char line[1024] = {0};
		if (regi_cancel_func(line, sizeof(line)) == -1) {
			return 1;  /* メニューに戻る */
		}
		/* 文字列が読み取れなかった場合 */
		if (sscanf(line, " %s", name) != 1) {  /* lineにはregi_cancel_funcでユーザ入力が入っている */
			printf("入力が読み取れませんでした。やり直してください。\n");
			continue;
		}
		printf("名前：%s\n", name);  /* 入力した名前の表示 */
		break;
	} return 0;
}


/*>>> 会員情報登録・編集時のクラス読み取り用関数 <<<*/
int select_class_func(int *class) {
	while (1) {
		printf("2.クラスを入力してください。(1~5級)\n");
		/* 入力がなかったとき */
		char line[128] = {0};
		if (regi_cancel_func(line, sizeof(line)) == -1) {
			return 1;
		}
		/* 数字が読み取れなかった場合 */
		if (regi_read_num(line, class) == -1) {	
			continue;
		}
		
		/* 数字がクラスの範囲外のとき */
		if (*class < 1 || *class > 5) {
			printf("クラスは1~5級までです。やり直してください。\n");
			continue;
		}
		printf("クラス：%d級\n", *class);
		break;
	} return 0;
}

/*>>> 会員情報登録・編集時の年齢読み取り用関数 <<<*/
int select_age_func(int *age) {
	while (1) {
		printf("3.年齢を入力してください。\n");
		/* 入力がなかったとき */
		char line[128] = {0};
		if (regi_cancel_func(line, sizeof(line)) == -1) {
			return 1;
		}
		/* 数字が読み取れなかった場合 */
		if (regi_read_num(line, age) == -1) {	
			continue;
		}
		printf("年齢：%d\n", *age);
		break;
	} return 0;
}

/*>>> 会員情報登録・編集時の性別読み取り用関数 <<<*/
int select_gender_func(int *gender) {
	while (1) {
		printf("4.性別を入力してください。\n"
			"1) 男性\n"
	      		"2) 女性\n"
			"3) その他\n"
		      );
		/* 入力がなかったとき */
		char line[128] = {0};
		if (regi_cancel_func(line, sizeof(line)) == -1) {
			return;
		}
		/* 数字が読み取れなかった場合 */
		if (regi_read_num(line, gender) == -1) {	
			continue;
		}
		/* 男性のとき */
		if (*gender == 1) {  
			printf("性別：男性\n");
			break;
		}
		/* 女性のとき */
		else if (*gender == 2) {
			printf("性別：女性\n");
			break;
		}
		/* その他の性別のとき */
		else if (*gender == 3) {
			printf("性別：その他\n");
			break;
		}
		else {
			printf("不適切な入力です。やり直してください。\n");
			continue;
		}
	}
}

/*>>> 会員情報登録・編集時の備考読み取り用関数 <<<*/
void create_note_func(char *note, int size, int edit_flag) {
	while (1) {
		if (edit_flag == 1) {
			printf("現在の備考：%s", note);
		}
		printf("5.備考を入力してください。未入力でも次に進みます。\n");
		if (fgets(note, size, stdin) == NULL || note[0] == '\n') {
			printf("備考を入力せずに進みます。\n");
			memset(note, 0, size);  /* 備考を初期化 */
			break;
		}
		printf("備考：%s", note);
		break;
	}
	return;
}
/*>>> すべての入会中の会員情報を表示する関数 <<<*/

/*
 * 引数の*member_num = -1のとき、会員情報を表示するだけ。
 * *member_num = 0のとき、追加でユーザは会員を選択して*member_numに代入する。
 */

int display_func(struct Member *head, int *member_num) {
	int current_num =  head->member_num;
	struct Member current_address = head;	
	struct Member next_address = head->next;
	printf("会員番号 名前 クラス 年齢 性別 備考\n");
	/* headから順番に会員番号を表示 */
	while (next_address != NULL) {
		/* 一人分の会員情報を一列で表示 */
		/* 性別は数値で入っているので分岐しないといけない */
		if (current_address.gender == 1) {
			printf("%d %s %d %d 男性 %s", current_address->membernum, current_address->name, current_address->class, current_address->age, current_address->note);
		}
		else if (current_address.gender == 2) {
			printf("%d %s %d %d 女性 %s", current_address->membernum, current_address->name, current_address->class, current_address->age, current_address->note);
		}
		else if (current_address.gender == 3) {
			printf("%d %s %d %d その他 %s", current_address->membernum, current_address->name, current_address->class, current_address->age, current_address->note);
		}
		else {
			printf("致命的なエラーが発生しました。");
			exit 1;
		}
		/* アドレスを次に動かす */
		current_address = next_address;
		next_address = next_address->next;
	}
	if (*member_num == 0) {
		printf("会員番号を選択してください。\n");
		char line[128] = {0};
		if (fgets(line, sizeof(line), stdin) == NULL || line[0] == NULL) {
			printf("入力がありませんでした。メニューに戻ります。\n");
			return 1;
		}
		/* 数字の読み取り（入力された数字の範囲の検証はsearch_funcで行う） */
		if (sscanf(line, " %d", member_num) != 1) {
			printf("数字が読み取れませんでした。メニューに戻ります。\n");
			return 1;
		}
return 0;
}

/*>>> 一人分の会員情報を表示する関数 <<<*/
void display_solo_func(struct Member *member) {
	printf("会員番号：%d\n"
		"名前：%s\n"
		"クラス：%d級\n"
		"年齢：%d歳\n"
		, member->member_num, member->name, member->class, member->age);
	/* 性別表示 */
	if (member->gender == 1) {
		printf("性別：男性\n");
	}
	else if (member->gender == 2) {
		printf("性別：女性\n");
	}
	else if (member->gender == 3) {
		printf("性別：その他\n");
	}
	/* 備考を表示 */
	printf("備考：%s", member->note);  /* 備考入力で改行が入っているので\nは不要 */
}

/* 会員番号から該当の会員を探してその会員のアドレスを代入する関数 */
int search_func(struct Member *head, int *member_num, struct Member *member_address) {
	int current_num =  head->member_num;
	struct Member current_address = head;	
	struct Member next_address = head->next;

	/* headから順番に指定した会員番号が見つかるまで検索 */
	while (currrent_num != *member_num) {
		if (current_address->next == NULL) {
			printf("指定した会員番号が見つかりませんでした。メニューに戻ります。\n");
			return 1;
		}	
		current_address = next_address;
		current_num = next_address->member_num;
		next_address = next_address->next;
	}
	member_address = current_address; 

return 0;
}

/*>>> 会員情報登録関数 <<<*/
/*
 * struct Member型のtempに登録に必要な情報を入れる。
 * 戻り値：登録が確定したら0、登録しなかったら1を返す
 *
 */
int regi_func(struct Member *head, int *load_count, struct Member *temp) {

	/*
	 * 入力された新規会員情報は一時的に保存するための変数に代入し、この関数の最後でまとめて本データに入力する処理をする
	 */
	
	
	printf("新しい会員を登録します。必要な情報を確認してください。未入力でEnterを押すと登録を中断します。\n");
	printf("必要な情報：1.名前 2.クラス 3.年齢 4.性別 5.備考\n\n");	
	
	/* 会員番号、入会状況、次のアドレスの入力 */
	temp->member_num = *loaded_count + 1;
	temp->is_deleted_account = 0;
	temp->next = NULL;

	/*>>> 1.名前の入力 <<<*/
	if (select_name_func(temp->name) == 1) {
		return 1;  /* 入力がなかったらメニューに戻る */
	}
	
	/*>>> 2.クラスの入力 <<<*/
	if (select_class_func(&temp->class) == 1) {
		return 1;  /* 入力がなかったらメニューに戻る */
	}

	/*>>> 3.年齢の入力 <<<*/
	if (select_age_func(&temp->age) == 1) {
		return 1;  /* 入力がなかったらメニューに戻る */
	}
	
	/*>>> 4.性別の入力 <<<*/
	if (select_gender_func(&temp->gender) == 1) {
		return 1;  /* 入力がなかったらメニューに戻る */
	}
			
	/*>>> 5.備考の入力 <<<*/
	create_note_func(temp.note, sizeof(temp->note), 0);
	
	/*>>> 入力情報の表示と登録の確認 <<<*/
	while (1) {	
		printf("以下の内容で新規会員情報を登録してもよろしいですか？[y/n]\n");
		/*>>> 登録する会員情報を表示 <<<*/
		display_solo_func(temp);
		
		/*>>> 新規会員情報登録の決定 <<<*/
		char line[128] = {0};
		char cmd_regi[128] = {0};
		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("入力がありませんでした。やり直してください。\n");
			continue;
		}
		if (sscanf(line, " %s", cmd_regi) != NULL) {
			if (cmd_regi[0] == 'y' || cmd_regi[0] == 'Y') {
				printf("登録処理を実行します。\n")
				break;  /* 登録が確定 */
			}
			else if (cmd_regi[0] = 'n' || cmd_regi[0] == 'N') {
				printf("会員情報の登録を中止します。\n");
				return 1;
				}
		}
		else {
			printf("不適切な入力です。やり直してください。\n");
			continue;
		}
	}
	return 0;
}


/*>>> 会員情報を編集する関数 <<<*/
int edit_func(struct Member *head) {
	printf("会員情報編集プログラムを起動します。\n");
	printf("誰の情報を編集しますか？\n");
	/* 現在入会中の会員情報を表示 */
       	int selected_member_num = 0;  /* display_funcを選択モードにする */
	/* selected_member_numに選択した会員番号を代入 */  
	if (display_func(head, &selected_member_num) == 1) {
		return;  /* 入力がなければメニューに戻る */
	}
       	struct Menber *selected_member_address;
	if (search_func(head, &selected_member_num, selected_member_address) == 1) {
		return;
	}
	printf("%sさんの情報を編集します。\n", selected_member_address->name);
	display_solo_func(selected_member_address);  /* 一人分の会員情報を表示 */
	printf("どの項目を編集しますか？\n"
		"1) 名前\n"
		"2) クラス\n"
		"3) 年齢\n"
		"4) 性別\n"
		"5) 備考\n"
	      );
	char line[128] = {0};
	if (edit_cancel_func(line, sizeof(line)) == -1) {
		return;  /* メニューに戻る */	
	}
	int edit_cmd = 0;
	if (edit_read_num(line, &edit_cmd) == -1) {
		return;  /* メニューに戻る */
	}	
	struct Member temp = {-1, {NULL}, -1, -1, -1, -1, {NULL}, NULL}; 
	/* 名前の編集 */
	if (edit_cmd == 1) {
		/* 入力がない場合 */
		if (select_name_func(temp.name) == 1) {
			printf("入力がありませんでした。編集を中断してメニューに戻ります。\n");
			return;  /* メニューに戻る */
		}
		selected_member_address->name = temp.name;
		printf("名前の編集を実行します。\n")
	}
	/* クラスの編集 */
	if (edit_cmd == 2) {
		/*入力がない場合 */
		if (select_class_func(&temp.class) == 1) {
			printf("入力がありませんでした。編集を中断してメニューに戻ります。\n");
			return;  /* メニューに戻る */
		}
		selected_member_address->class = temp.class;
		printf("クラスの編集を実行します。\n");
	}
	/* 年齢の編集 */
	if (edit_cmd == 3) {
		/*入力がない場合 */
		if (select_age_func(&temp.age) == 1) {
			printf("入力がありませんでした。編集を中断してメニューに戻ります。\n");
			return;  /* メニューに戻る */
		}
		selected_member_address->age = temp.age;
		printf("年齢の編集を実行します。\n");
	}
	/* 性別の編集 */
	if (edit_cmd == 4) {
		/*入力がない場合 */
		if (select_gender_func(&temp.gender) == 1) {
			printf("入力がありませんでした。編集を中断してメニューに戻ります。\n");
			return;  /* メニューに戻る */
		}
		selected_member_address->class = temp.class;
		printf("クラスの編集を実行します。\n");
	}



	/*===== 編集中 =====*/
	
}
/*>>> 会員情報を削除（退会）する関数 <<<*/
void dele_func(struct Member *head) {
	printf("退会する会員を選んでください。\n");
	/* 現在入会中の会員情報を表示 */
	int selected_member_num = 0;  /* display_funcを選択モードにする */
	display_func(&selected_member_num);
	struct Member *selected_member_address;
	if (search_func(head, selected_member_num, selected_member_address) == 1) {
		return;
	}
	
	/*===== 編集中 =====*/

	/* ユーザ入力と入力検証 */

	printf("~さんを退会させます。再入会する場合は新規登録が必要になります。よろしいですか？\n");  /*===== 変数で~を置き換える =====*/
	/* ユーザ入力と入力検証 */
	/* 退会する場合は名前を伏せ字に置き換え、備考を削除する */
	printf("退会処理が完了しました。メニューに戻ります。\n");
	printf("退会処理を中止します。メニューに戻ります。\n");
}

/*>>> 会員情報を閲覧する関数 <<<*/
void view_func(struct Member *head) {
	printf("現在入会中の会員情報を表示します。\n");
	/* 現在入会中の会員情報を表示 */
	int selected_member_num = -1;  /* display_funcを閲覧モードにする */
	display_func(head, &selected_member_num);
	printf("Enterでメニューに戻ります。\n");
	char line[32] = {0};
	/* Enter入力でメニューに戻る */
	fgets(line, sizeof(line), stdin);  /* Enterさえ押せば次に進む */
	return;	
}
int file_write_func(head) {
/*===== 編集中 =====*/
}

int main() {
/*>>> 変数の定義 <<<*/
struct Member *head = NULL;  /* 最初のメンバーのアドレス */
int loaded_count = -1;  /* ファイルから読み出された会員数を数える変数、初期値は-1とする */
/* 新規会員情報を一時的に保存する変数 */
struct Member temp; 

/*>>> ファイル読み出し <<<*/
	load_func();  /*=== ファイル読み出し用関数を起動 ===*/

/*>>> プログラム起動時のプロンプト <<<*/
	printf("会員情報プログラムを起動しました。\n");

/*>>> メニュー画面 <<<*/
	/* 繰り返し操作できるようにwhileループさせる */
	while (1) {
		/* ループ時にtempを初期化 */
		memset(temp, 0, sizeof(struct Member);
		printf("どの操作をしますか？\n");
		printf("1) 登録\n"
			"2) 編集\n"
		       	"3) 削除\n"
			"4) 閲覧\n"
			"q) やめる\n"
	      );

		/* 入力がなかった場合 */
		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("入力がありませんでした。やり直してください。\n");
			continue;
		}
		
		/* 入力が文字列かどうか確認 */
		char cmd_menu[32] = {0}
		sscanf(line, " %s", cmd_menu);  /* 読み取れないとき、cmd_strは初期化されたまま */ 
		
		/* qまたはQが入力されたとき */
		if (cmd_menu[0] == 'q' || cmd_menu[0] == 'Q') {
			printf("プログラムを終了します。\n");
			break;
		}
		
		/* 入力された数字を読み取る */
		int cmd_num = 0;
		sscanf(line, " %d", &cmd_num);  /* 読み取れないとき、cmd_numは0のまま */
		
		if (cmd_num == 0) {  /* 数字が読み取れないとき */
			printf("数字が読み取れませんでした。やり直してください。\n");
			continue;
		}
		else if (cmd_num == 1) {
			printf("会員情報登録プログラムを起動します。\n");
			/*=== 会員登録用関数を起動 ===*/
			if (regi_func(head, &loaded_count, &temp) == 1) {
				continue;
			}
			/* ここでtempの会員情報をmallocで新しく確保したメモリに保存する */
			temp_to_newmem();  /*===== 編集中 =====*/
			file_write_func(head);  /*=== ファイル書き込み関数起動 ===*/

		}
		else if (cmd_num == 2) {
			printf("会員情報編集プログラムを起動します。\n");
		      	/*=== 会員編集用関数を起動 ===*/
			if (edit_func(head, &temp) == 1) {
				continue;
			}
			temp_to_edit();  /*===== 編集中 =====*/
			file_write_func(head);  /*=== ファイル書き込み関数起動 ===*/
		}
		else if (cmd_num == 3) {
			printf("会員情報削除プログラムを起動します。\n");
		       	/*=== 会員削除用関数を起動 ===*/
			if (dele_func(head, &temp) == 1) { 
				continue;
			}
			temp_to_delete();  /*===== 編集中 =====*/
			file_write_func(head);  /*=== ファイル書き込み関数起動 ===*/
		}
		else if (cmd_num == 4) {
			printf("会員情報閲覧プログラムを起動します。\n");
			view_func(head);  /*=== 会員閲覧用関数を起動 ===*/
			continue;
		}
		else {  /* その他の数字が入力されたとき */
			printf("不適切な入力です。やり直してください。\n");
			continue;
		}
	}  /* whileループの終わり */
	return 0;
}  /* main関数の終わり */

/*
 * メモ：
 * 作成途中だがスパゲティコードになってきたので新しく作り直すことにする。特に何も考えず、いつもの学習用コードを書く感じで始めたのが失敗の原因だった。最後の方はどの関数が何を操作しているのかで混乱して作業効率が極端に落ちていた。
 * 書き始めたときは今まで書いた中でも長いプログラムになるとは思っていたが、書いている途中で必要だと思う処理が急激に増えていってしまった。これくらいの規模のアプリでも予想外に必要な処理の数が多く複雑だった。モジュールの概念の重要性がよくわかった。次回からは今回の経験を元に、プログラムを書き始める前に仕様をしっかり決めて、複数ファイルに役割ごとに分けるようにしたい。今回のこのファイルは失敗例として残しておきたいと思う。
 * 初期のプログラムではユーザ入力のlineをグローバル変数にしていたが、グローバル変数はシステムが大きくなるほどバグの原因になったときにデバッグが大変なので、ローカル変数にした。これを修正するのも面倒だったので、規模が大きいプログラムを書く前に学べて良かったと思う。
 * ソースコードの可読性も上げたい。関数の使い方をコメントでわかりやすく書いて、変数や関数名の統一性も気をつけたい。
 * まだプログラムに追加したい機能は思いつくが、際限がないので今回決めた最低限の機能に留めて次回作成しようと思う。
 * 
 * 新機能を追加するとしたら：
 * ・誕生日の項目を実装し、誕生日ごとに年齢を自動で更新する。（現在の仕様だと年齢を手入力で更新しないといけないし、いつ年齢を更新すべきかもわからない）
 * -> 起動時に前回起動時の日付と現在の日付を取得し、メンバーの誕生日と比較して、誕生日が過ぎた回数だけ年齢を加算する。
 * ・細かなUIの調整、絞り込み機能（指定したクラスのメンバーだけ表示する）など
 * ・統計機能（退会済みのメンバーも含めた統計を活用できるようにする）、項目の追加（入会日や退会日など、経営の参考になりそうな情報は貯めておきたい）
 */
