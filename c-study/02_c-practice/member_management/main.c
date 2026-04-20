#include "member.h"

/*
 * 感想などはこのファイルの最後にあります。
 *
 * === 登録した会員情報(スイミングスクール想定)を閲覧できるプログラム ===
 * 
 * 仕様：
 * 会員情報の登録・編集・削除・閲覧が可能。
 * 会員情報をファイルに保存して起動時に自動で読み込む。
 * 退会（削除）された会員はそのまま退会済みの会員としてファイルに残る。
 * 退会した会員は氏名を伏せ字にして備考を削除する。
 * 新規会員の会員番号はファイルに存在する最後尾の会員番号に+1して登録する。
 * ファイル書き込み時は既存のファイルに上書き保存する。
 *
 * 登録情報：
 * 会員番号・名前・クラス（1~5級,上限は変更可能）・年齢・性別・備考 
 */

int main() {
	/* プログラム開始メッセージを表示 */
	start_message_ui();

	/*>>> ファイル読み込み <<<*/
	/*
	 * ファイルの読み込みを試行する。
	 * ファイルがなければそのままメニューに進む(会員登録時にファイルが作成される)。
	 * ファイルがあれば読み込み、読み込めたデータ件数を表示。 その後メニューに移動する。
	 */
	read_file_ui(READ_START, 0);
	struct Member *head = NULL;
	int loaded_count = 0;
	/* head(会員番号1番のアドレス)を得る(headはNULLでなければこれ以降変わらない) */
	/* 会員番号1番が存在しない場合、head == NULL */
	/* 読み込めたデータ件数を表示 */
	enum ReadFileStatus read_status = read_file(&head, &loaded_count);
	read_file_ui(read_status, loaded_count);
	while (1) {
		struct Member temp = {0};
		/*>>> メニュー画面 <<<*/
		/*
		 * ユーザにメニュー画面を表示する。
		 * 登録、編集、削除、閲覧、やめる（プログラム終了）が選択可能
		 */
		enum MenuSelectCmd cmd = menu_ui();

		/*>>> 新規会員登録 <<<*/
		switch (cmd) {
			case SELECT_REGISTER:
				register_ui(REGISTER_START, 0);
				/* 新規会員の会員番号をtemp.member_numに入れる */
				if (register_get_new_member_num(head, &temp) == false) {
					register_ui(REGISTER_ERROR_MAX_MEMBER, 0);
					continue;
				}
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
				/* 会員登録処理を実行 */
				enum RegisterStatus exe_status = register_execute(&head, &temp);
				switch (exe_status) {
					case REGISTER_SUCCESSED:
						break;
					case REGISTER_ERROR_UNKNOWN:
						register_ui(REGISTER_ERROR_UNKNOWN, 0);
						continue;
					case REGISTER_ERROR_MEMORY:
						register_ui(REGISTER_ERROR_MEMORY, 0);
						continue;
					default:
						assert(0 && "[DEBUG] register_executeの戻り値で問題が発生しました。");

				}
				register_ui(REGISTER_COMPLETED, 0);
				/* ファイル書き込み */
				write_file_ui(WRITE_START);
				if (write_file(head) == false) {
					write_file_ui(WRITE_FAILED);
					continue;
				}
				
				write_file_ui(WRITE_COMPLETED);
				continue;
			
			/*>>> 会員情報編集 <<<*/
			/*
			 * 入会中の全会員を表示後、ユーザの選択した会員情報を編集する。
			 * 編集終了後、ファイルに書き込みを行う。
			 */
			case SELECT_EDIT:
				if (head == NULL) {
					edit_ui(EDIT_NO_MEMBER, 0);
					continue;
				}
				edit_ui(EDIT_START, 0);
				/* 入会中の会員が存在しない（全員退会済みのとき） */
				if (display_member(head) == NOT_FOUND) {
					edit_ui(EDIT_NO_MEMBER, 0);
					continue;
				}
					
				/* temp.member_num に会員番号を渡す */
				if (edit_ui(EDIT_SELECT_MEMBER, &temp) == CANCEL) {
					edit_ui(EDIT_CANCEL, 0);
					continue;
				}
				struct Member *member_address = NULL;
				/* member_addressに編集する会員のアドレスを渡す */
				enum SearchMemberStatus search_status = search_member_address(head, temp.member_num, &member_address);
				switch (search_status) {
					case FOUND:
						break;
					case NOT_FOUND:
						edit_ui(EDIT_MEMBER_NOT_FOUND, 0);
						continue;
					case IS_DELETED:
						edit_ui(EDIT_MEMBER_IS_DELETED, 0);
						continue;
					default:
						assert(0 && "search_member_addressの戻り値で問題が発生しました。");
				}
				/* tempに編集予定の会員情報をコピー */
				temp = *member_address;
			        /* 編集項目を選んでtempに入れる */
				edit_ui(EDIT_ITEM, &temp);
				if (edit_ui(EDIT_CONFIRM, &temp) == CANCEL) {
					edit_ui(EDIT_CANCEL, 0);
					continue;
				}
				/* 編集予定の会員情報にtempをコピー(編集完了) */
				*member_address = temp;
				edit_ui(EDIT_COMPLETED, 0);
				/* ファイル書き込み */
				write_file_ui(WRITE_START);
				if (write_file(head) == false) {
					write_file_ui(WRITE_FAILED);
					continue;
				}
				write_file_ui(WRITE_COMPLETED);
				continue;
		
			/*>>> 会員情報削除 <<<*/
			/* 
			 * 入会中の全会員を表示し、ユーザが指定した会員の削除を実行する。
			 * 会員情報削除は該当会員の名前と備考を伏せ字に置き換える。
			 * その後ファイルに書き込みを行う。 
			 */
			case SELECT_DELETE: 
				if (head == NULL) {
					delete_ui(DELETE_NO_MEMBER, 0);
					continue;
				}
				delete_ui(DELETE_START, 0);
				/*  表示できる会員がいない場合、メッセージを出してメニューに戻る 			  
				   （全ての会員が退会している場合）*/
				if (display_member(head) == NOT_FOUND) {
					delete_ui(DELETE_NO_MEMBER, 0);
					continue;
				}
				/* temp.member_num に会員番号を渡す */
				if (delete_ui(DELETE_SELECT_MEMBER, &temp) == CANCEL) {
					delete_ui(DELETE_CANCEL, 0);
					continue;
				}
				struct Member *delete_member_address = NULL;
				/* delete_member_addressに選択した会員情報のアドレスを渡す */
				switch (search_member_address(head, temp.member_num, &delete_member_address)) {
					case FOUND:
						break;
					case NOT_FOUND: 
						delete_ui(DELETE_MEMBER_NOT_FOUND, 0);
						continue;
					case IS_DELETED:
						delete_ui(DELETE_IS_ALREADY_DELETED, 0);
						continue;
					default:
						assert(0 && "[DEBUG] search_member_addressの戻り値で問題が発生しました。");
				}
				if (delete_ui(DELETE_CONFIRM, delete_member_address) == CANCEL) {
					delete_ui(DELETE_CANCEL, 0);
					continue;
				}
				/* 会員情報の名前を伏せ字に置き換え、備考を削除 */
				delete_execute(delete_member_address);
				delete_ui(DELETE_COMPLETED, 0);
				/* ファイル書き込み */
				write_file_ui(WRITE_START);
				if (write_file(head) == false) {
					write_file_ui(WRITE_FAILED);
					continue;
				}
				write_file_ui(WRITE_COMPLETED);
				continue;
		
			/*>>> 会員情報閲覧 <<<*/
			/* 入会中の会員情報表示後、ユーザがEnterを押すとメニューに戻る */
			case SELECT_VIEW: 
				view_ui(VIEW_START);
				if (display_member(head) == NOT_FOUND) {
					view_ui(VIEW_NO_MEMBER);
					continue;
				}
				view_ui(VIEW_WAIT);
				view_ui(VIEW_EXIT);
				continue;
		

			/*>>> プログラム終了 <<<*/
			/* 全メモリを解放後にプログラムを終了 */
			case SELECT_QUIT:
				quit_message_ui();
				free_all_memory(head);
				exit(0);
			default:
				assert(0 && "[DEBUG] menu_uiの戻り値で問題が発生しました。");
		}
		break;
	}
	return 0;
}

/*
 * メモ：
 * 前回の経験(failed_structure_member.c)を活かして以下の点に注意して制作することにした。
 * 以下が失敗したバージョンのURL
 * https://github.com/kazu260111/c-study/blob/main/c-study/02_c-practice/failed_structure_member.c
 *
 * 1. プログラムの実装方法と全体の流れの確認
 *
 *  - 前回はプログラムを書きながらどう実装すればいいか考えていたが、先に技術的な実装方法を決めていないと途中で必要な変数が増えたりして広範囲の修正が必要になったりした。
 *    必要な機能を考えて先に実装方法を決定し、制作途中で実装方法がころころ変わることがないようにする。
 *  -> 実際は途中で一部のフラグをenum型にすべきだということに気づいたり、デバッグメッセージを追加したりと追加が必要なところが多かった。そのあたりは経験が重要だと思うので、次回から意識していきたい。
 *
 * 2. 必要な関数の洗い出しとヘッダファイルの作成
 *
 *  - この過程で関数の引数と戻り値をはっきりさせ、関数を部品として使えるようにする。
 *    関数はこの段階で完成させなくても、引数と戻り値がはっきりすればよいことにした。
 *  - 関数を作るときはUI部分を担当する関数とロジック部分を担当する関数で役割を完全に分けるように気をつけた。
 *  -> 実際はmain関数を書きながらさらに改善箇所が見つかったが、関数の役割分担をしたおかげであまり混乱せずに修正することができた。
 *
 * 3. main関数で全体の流れをはっきりさせる
 *
 *  - 関数を部品と使えるようにすれば、main関数で最後までの大まかな流れが作れる。中身が空の関数を準備してとりあえず動くようにする。
 *  - 一つの関数が完成したらとりあえずmainに入れてmakeでコンパイルし動作確認をする。作成した関数の部分が正常に動くことを確認してから次の関数に取り掛かる。
 *  -> 実際はほとんどすべての関数を実装してからmakeでデバッグを始めていた。ロジックは難しくなかったので、いちいちコンパイルするのが面倒になってしまっていた(とりあえず完成してからエラーをLLMに投げればいいと思っていた)
 *     また、モジュール化について理解していたので、修正するとしても前回のように全体に波及してやり直しにはならないだろうと思っていた。しかし、mainから正しい流れで関数が実行されているか確認するためにデバッグメッセージを使いながら何度もコンパイルすべきだと考え直したので、次回から気をつけたいと思う。
 * 
 * 多かったエラー
 *
 * - セミコロンのつけ忘れ
 * - カッコの数が合わない
 * - 未定義のenum型変数
 *   - わかりやすくするためにenum型にしたが、あとで定義しようとしていたら忘れた
 * - 関数の引数の数を間違える
 *   - 二つ引数が必要な関数なのに一つしか必要ないときに、二つ目を書き忘れた(0などを入れる必要がある)
 * - 一部ロジックをコピペして使いまわすときに変数の名前を変え忘れた
 * - 単なるタイプミス
 * 
 * 実際にあったバグの例
 *
 * - 備考を未記入のままで進めると、データ書き込みのときに書き込むデータの種類が合わなくなる
 *   - 一人につき7種のデータをファイルに書くが、備考が未記入だと備考を除いた6種類のデータしか書き込まなくなるので読み取れなくなる
 *   -> 備考が未記入の場合"なし"と書き込むことにした。会員情報削除時も、備考を削除ではなく伏せ字にすることにした。
 *
 * - 会員登録してもそのままだと編集、削除、閲覧ができない
 *   -> 入会時に入会フラグを立てるのを忘れていた 
 *
 * - tsvファイルを読み込んでも読み込んだデータ件数が0件と表示される
 *   -> read_file_ui(read_status, loaded_count); のread_status部分にread_file(&head, &loaded_count)を直接入れていたが、実際の動作だとread_file_uiの引数のうちloaded_countが先に評価されるせいで、read_fileでloaded_countが更新される前にloaded_count = 0としてread_file_uiに読み込まれてしまっていた。
 * 
 * - 名字と名前を両方入力する(空白を入力する)と片方しか読み取れない問題
 *   -> strcpyを使うようにして空白を複数入れるような名前でも読み取れるようにした
 *
 * 感想
 * - 最初は数日もかからないと思っていたが、途中で必要な関数が増えて混乱しがちだった。設計が最も重要なフェーズだと理解した。
 *   チームで制作するならコードの可読性や変更のしやすさなども重要なので、優れた設計は意識していきたい。
 * - 変数名や関数名、ヘッダファイルの説明などを考えて決めておかないと、デバッグのとき苦労するとわかった。
 *   この規模のプログラムでも苦労するなら、規模の大きいものならなおさら設計から綿密に取り組むべきだと思った。
 * - バグの検証ではなかなか原因が見つからず、LLMも使用したが時間がかかってしまった。
 *   やはり関数を制作するたびにコンパイルして、どこで問題が発生したのかわかるようにした方が総合的な制作時間を短くできそうだと思った。
 * - ロジックはできるだけ自力で作成したが、LLMに評価してもらうと自作のコードは冗長だったり動作するが今後の拡張性に問題があると言われることがあった。
 *   基本的な文法は理解したのでできるだけ自力で書いたほうが学習になると考えて制作していたが、今後は他人の優れたコード例を見て真似ることも意識したいと思った。
 * - 製品として出すなら徹底的にバグチェックリストを作成して一つずつ潰していくべきだが、今回は学習用である程度の動作を確認できたので、一旦他の課題に挑戦したい。
 */
