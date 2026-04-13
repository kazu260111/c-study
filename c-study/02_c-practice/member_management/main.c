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

int main() {
	startup_ui();
	read_file_ui(READ_START);
	struct Member *head = NULL;
	int loaded_count = 0;
	/* headを得る(headはこれ以降変わらない) */
	read_file_ui(read_file(&head, &loaded_count), loaded_count);
	while (1) {
		struct Member temp = {0};
		enum MenuSelectCmd cmd = menu_ui();
		/*>>> 新規会員登録 <<<*/
		if (cmd == SELECT_REGISTER) {
			register_ui(REGISTER_START, &temp);
			if (register_ui(REGISTER_NAME, &temp) == CANCEL) {
				continue;
			}
			if (register_ui(REGISTER_CLASS, &temp) == CANCEL) {
				continue;
			}
			if (register_ui(REGISTER_AGE, &temp) == CANCEL) {
				continue;
			}
			if (register_ui(REGISTER_GENDER, &temp) == CANCEL) {
				continue;
			}
			register_ui(REGISTER_NOTE, &temp);
			display_solo_member(&temp);
			if (register_ui(REGISTER_CONFIRM, &temp) == CANCEL) {
				continue;
			}
			register_execute(head, &temp);
			/*
			上の関数は以下を含む(仮のメモ)
			malloc_new_address();
			search_tail();
			add_tail_next();
			temp_to_new_address(head, &temp);
			*/
			register_ui(REGISTER_COMPLETED, 0);
			write_file_ui(WRITE_START);
			if (write_file(head) == false) {
				write_file_ui(WRITE_FAILED);
				continue;
			}
			
			write_file_ui(WRITE_COMPLETED);
			continue;
		}
		/*>>> 会員情報編集 <<<*/
		else if (cmd == SELECT_EDIT) {
			edit_ui(EDIT_START, 0);
			display_member(head);
			/* temp.member_num に会員番号を渡す */
			if (edit_ui(EDIT_SELECT_MEMBER, &temp) == CANCEL) {
				edit_ui(EDIT_CANCEL, 0);
				continue;
			}
			struct Member *member_address;
			if (search_member_address(head, temp.member_num, &member_address) == NOT_FOUND) {
				edit_ui(EDIT_MEMBER_NOT_FOUND, 0);
				continue;
			}
			/* tempに編集予定の会員情報をコピー */
			temp = *member_address;
			display_solo_member(&temp);
			edit_ui(EDIT_ITEM, &temp);  /* 編集項目を選んでtempに入れる */
			if (edit_ui(EDIT_CONFIRM, 0) == CANCEL) {
				edit_ui(EDIT_CANCEL, 0);
				continue;
			}
			/* 編集予定の会員情報にtempをコピー(編集完了) */
			*member_address = temp;
			edit_ui(EDIT_COMPLETED, 0);
			write_file_ui(WRITE_START);
			if (write_file(head) == false) {
				write_file_ui(WRITE_FAILED);
				continue;
			}
			write_file_ui(WRITE_COMPLETED);
			continue;
		}
		/*>>> 会員情報削除 <<<*/
		else if (cmd == SELECT_DELETE) {
			delete_ui(DELETE_START, 0);
			display_member(head);
			/* temp.member_num に会員番号を渡す */
			if (delete_ui(DELETE_SELECT_MEMBER, &temp) == CANCEL) {
				delete_ui(DELETE_CANCEL, 0);
				continue;
			}
			/* tempに選択した会員情報のアドレスを渡す */
			if (search_member_address(head, temp.member_num, &temp) == NOT_FOUND) {
				delete_ui(DELETE_MEMBER_NOT_FOUND);
				continue;
			}
			display_solo_member(&temp);
			if (delete_ui(DELETE_CONFIRM, &temp) == CANCEL) {
				delete_ui(DELETE_CANCEL, 0);
			}
			delete_execute(&temp);
			delete_ui(DELETE_COMPLETED, 0);
			write_file_ui(WRITE_START);
			if (write_file(head) == false) {
				write_file_ui(WRITE_FAILED);
				continue;
			}
			write_file_ui(WRITE_COMPLETED);
			continue;
		}
		/*>>> 会員情報閲覧 <<<*/
		else if (cmd == SELECT_VIEW) {
			view_ui(VIEW_START);
			display_member(head);
			view_ui(VIEW_WAIT);
			view_ui(VIEW_EXIT);
			continue;
		}

		/*>>> プログラム終了 <<<*/
		else if (cmd == SELECT_QUIT) {
			quit_message_ui();
			free_all_memory(head);
			break;
		}
		continue;
	}
	return 0;
}







	


		

	
	

	return 0;
}

/*
 * メモ：
 * 以前の経験を活かして以下の点に注意して制作することにした。
 * 1. プログラムの実装方法と全体の流れの確認
 *  - 前回はプログラムを書きながらどう実装すればいいか考えていたが、先に技術的な実装方法を決めていないと途中で必要な変数が増えたりして広範囲の修正が必要になったりした。
 *    必要な機能を考えて先に実装方法を決定し、制作途中で実装方法がころころ変わることがないようにする。
 *
 * 2. 必要な関数の洗い出しとヘッダファイルの作成
 *  - この過程で関数の引数と戻り値をはっきりさせ、関数を部品として使えるようにする。
 *    関数はこの段階で完成させなくても、引数と戻り値がはっきりすればよいことにした。
 *  - 関数を作るときはUI部分を担当する関数とロジック部分を担当する関数で役割を完全に分けるように気をつけた。
 *  -> 実際はmain関数を書きながらさらに改善箇所が見つかったが、関数の役割分担をしたおかげであまり混乱せずに修正することができた。
 *
 * 3. main関数で全体の流れをはっきりさせる
 *  - 関数を部品と使えるようにすれば、main関数で最後までの大まかな流れが作れる。中身が空の関数を準備してとりあえず動くようにする。
 *  - 一つの関数が完成したらとりあえずmainに入れてmakeでコンパイルし動作確認をする。作成した関数の部分が正常に動くことを確認してから次の関数に取り掛かる。
 * 
 */
