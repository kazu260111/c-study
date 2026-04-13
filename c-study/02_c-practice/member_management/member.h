#ifndef MEMBER_H
#define MEMBER_H

#include <stdbool.h>



/*>>>>> 構造体の定義 <<<<<*/
struct Member {
        int member_num;  /* 会員番号 */
        char name[1024];  /* 氏名 */
        int class_num;  /* クラス（1~5級） */
        int age;  /* 年齢 */
        char gender[256];  /* 男性、女性、その他のいずれかの文字列を入れる */
	bool is_deleted_account;  /* 入会中なら0,退会済みなら1 */
        char note[4096];  /* 備考 */
        struct Member *next;  /* 次のメンバーのアドレス(プログラム起動ごとに毎回更新される */
};
/*>>>>> 列挙型の定義 <<<<<*/
/* メニューで使用 */
enum MenuSelectCmd {
	SELECT_REGISTER = 1,
	SELECT_EDIT = 2,
	SELECT_DELETE = 3,
	SELECT_VIEW = 4,
	SELECT_QUIT = 5
};

/* ファイル読み込み時使用 */
enum ReadFileStatus {
	READ_START = 0,
	READ_SUCCESS = 1,
	READ_FAIL = 2,
	READ_EMPTY_FILE = 3
};

enum SearchMemberStatus {
	FOUND = 0,
	NOT_FOUND = 1,
};

enum CancelCheck {
	CONTINUE = 0,
	CANCEL = 1
};

enum RegisterStep {
	REGISTER_START = 0,
	REGISTER_NAME = 1,
	REGISTER_CLASS = 2,
	REGISTER_AGE = 3,
	REGISTER_GENDER = 4,
	REGISTER_NOTE = 5,
	REGISTER_CONFIRM = 6
};

/*>>>>> 関数のプロトタイプ宣言と各ファイルの説明 <<<<<*/
/*>>> tool.c <<<*/
/* ファイル説明:
 * よく使う便利な関数をまとめている 
 */

/*
 * 使い方: - ユーザの入力を引数とした配列に渡す
 * 引数: char line  - ユーザ入力を受け取りたい配列名
 *       int size  - 第一引数の配列の大きさ(あらかじめsizeof(line)で求めてこの引数として使用する) 
 * 戻り値: true  - ユーザ入力を配列に渡せた場合
 *         false  - ユーザ入力を配列に渡せなかった場合
 */
bool get_cmd(char line, int size);

/*
 * 使い方: - 指定した配列に数字があれば読み取って、引数のint型ポインタに数字を渡す。
 * 引数: int *num  - 数字入力を受け取りたい変数のアドレス
 * 戻り値: true  - 正常に数字を渡せた場合
 *         false  - 数字を渡せなかった場合
 */
bool check_line_is_num(char line, int *cmd );

 /*
  * 使い方:  - 第1引数の配列に文字列があれば読み取って、第2引数の配列に渡す。(入力文字列先頭の空白を削除する)
  * 引数:  char line  - 読み取る配列の配列名
  * 	   char cmd_line  - 読み取れた文字列を渡す配列名
  * 戻り値: true  - 正常に文字列を渡せた場合
  *         false  - 文字列を渡せなかった場合
  */
bool check_line_is_str(char line, char cmd_line);

/*>>> ui.c <<<*/
/* ファイル説明:
 * ユーザに表示するUIを扱う関数をまとめる
 */

/*
 * void start_message_ui()
 * 使い方: - プログラム起動後に使用し、ファイル読み込み時のUIを表示する
 * 引数: なし
 * 戻り値: なし         
 */
void start_message_ui();  

/*
 * void read_file_ui(enum ReadFileStatus status, int loaded_count)  
 * 使い方: - startup_ui実行後に使用する。ファイルの読み込みに関するUIを表示する
 * 引数: enum ReadFileStatus status  - 出すメッセージを選択するためのステータスコード
 *       int loaded_count  - ファイルから読み出した人数(status=Read_SUCCESSのときのみ使用)
 * 戻り値: なし
 */
void read_file_ui(enum ReadFileStatus status, int loaded_count);

/*
 * 使い方: - 起動時のファイル読み込み後に使用し、メインメニューのUIを表示し、ユーザのメニュー選択コマンドを返す
 * 引数: なし
 * 戻り値: enum MenuSelectCmd
 */
enum MenuSelectCmd menu_ui();

/*
 * 使い方: - 新規会員登録画面のメッセージを表示する
 * 引数: enum RegisterStep  - 会員登録進行状況 
 *       struct Member *temp  - 登録するデータの仮置き場のアドレス
 * 戻り値: enum CancelCheck
 */
enum CancelCheck register_ui(enum RegisterStep, struct Member *temp);

/*
 * void edit_ui(struct Member *head, struct Member *temp)  
 * 使い方: - メインメニューで使用し、会員情報編集画面を表示する。
 * 引数: struct Member *head  - 会員番号1番のアドレス
 *       struct Member *temp  - 編集する予定の新規会員情報を一時保存するアドレス
 * 戻り値: なし
 */
void edit_ui(struct Member *head, struct Member *temp);

/*
 * void delete_ui(struct Member *head, struct Member *temp)  
 * 使い方: - 会員情報削除画面に移動
 * 引数: struct Member *head  - 会員番号1番のアドレス
 *       struct Member *temp  - 削除する予定の新規会員情報を一時保存するアドレス
 * 戻り値: なし
 */
void delete_ui(struct Member *head, struct Member *temp);  

/*
 * void view_ui(struct Member *head)  
 * 使い方: - 会員情報閲覧画面に移動
 * 引数: struct Member *head  - 会員番号1番のアドレス
 * 戻り値: なし
 */
void view_ui(struct Member *head);  

/*
 * 使い方: - プログラム終了手続きに移動
 * 引数: なし
 * 戻り値: なし
 */
void quit_message_ui();  

/*
 * void write_file_ui(struct Member *head)  
 * 使い方: -
 * 引数:   - 
 * 戻り値: なし
 */
void write_file_ui(struct Member *head);

/*
 * void newfile_ui()  
 * 使い方: - セーブファイルが存在しないときに使用するUI
 * 引数:   - 
 * 戻り値: なし
 */
void newfile_ui();

/*
 * void loaded_emptyfile_ui()  
 * 使い方: - 空のファイルが見つかった場合に使用するUI
 * 引数:   - 
 * 戻り値: なし
 */
void loaded_emptyfile_ui();

/*>>> member.c <<<*/
/* ファイル説明:
 * 会員情報の検索、変更を行う関数をまとめる 
 */

/*
 * 使い方: - 第2引数で指定した会員番号のアドレスを第3引数に渡す
 * 引数: struct Member *head  - 会員番号1番のアドレス
 *       int member_num  - アドレスを知りたい会員の番号
 *       struct Member **member_address  - 見つけた会員のアドレスを渡す先
 * 戻り値: enum SearchMemberStatus
 *         
 */
enum SearchMemberStatus search_member_address(struct Member *head, int member_num, struct Member **member_address);  

/*
 * 使い方: - プログラム終了前にすべてのメモリを解放する
 * 引数: struct member *head  - 会員番号1番のアドレス
 * 戻り値: なし
 */
void free_all_memory(struct member *head);

/*>>> file_io.c <<<*/
/* ファイル説明:
 * ファイルに読み書きする関数をまとめる
 */

/*
 * 使い方: -
 * 引数:   - 
 * 戻り値: enum ReadFileStatus
 */
enum ReadFileStatus read_file(struct Member **head, int *loaded_count);

/*
 * int write_file(struct Member *head )  
 * 使い方: -
 * 引数:   - 
 * 戻り値: 0 or 1  - 成功した場合0、失敗した場合1
 */
int write_file(struct Member *head);

/*
 * int is_file_empty()  
 * 使い方: - 読み込むファイルが空かどうかチェックする関数
 * 引数:   - 
 * 戻り値: 0  - 成功した場合
 *         1  - 失敗した場合
 */
int is_file_empty();


 

#endif
