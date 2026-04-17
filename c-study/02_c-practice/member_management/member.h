#ifndef MEMBER_H
#define MEMBER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <assert.h>

/* 最大クラスを変えるときはここを編集(1級~MAX_CLASS級までのクラスを設定可能) */
#define MAX_CLASS 5
/*--- 会員数の最大人数を変えたい場合ここを変える ---*/
#define MAX_MEMBER 100 

/* 列挙体の定義(構造体で使用するもの) */
enum DelitedAccount {
	ACCOUNT_IS_ACTIVE = 1,
	ACCOUNT_IS_DELETED = 2
};

/*>>>>> 構造体の定義 <<<<<*/
struct Member {
        int member_num;  /* 会員番号 */
        char name[1024];  /* 氏名 */
        int class_num;  /* クラス（1~5級） */
        int age;  /* 年齢 */
        char gender[256];  /* 男性、女性、その他のいずれかの文字列を入れる */
	enum DelitedAccount is_deleted_account;  /* ACCOUNT_IS_ACTIVE, ACCOUNT_IS_DELETED のいずれか */
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
	READ_SUCCESSED = 1,
	READ_FAILED = 2,
	READ_EMPTY_FILE = 3,
	READ_ERROR_MEMORY = 4,
	READ_ERROR_ABNORMAL_DATA = 5,
	READ_MAX_MEMBER = 6
};

/* ファイル書き込み時使用 */
enum WriteFileStatus {
	WRITE_START = 0,
	WRITE_FAILED = 1,
	WRITE_COMPLETED = 2
};

/* search_member_address関数, display_member関数,display_solo_member関数で使用 */
enum SearchMemberStatus {
	FOUND = 0,
	NOT_FOUND = 1,
	IS_DELETED = 2
};

/* search_tail関数で使用 */
enum SearchTailStatus {
	SEARCHTAIL_SUCCESSED = 0,
	SEARCHTAIL_FAILED = 1,
	SEARCHTAIL_NO_MEMBER = 2
};

/* 未入力時キャンセルする場面で使用 */
enum CancelCheck {
	CONTINUE = 0,
	CANCEL = 1
};

/* yes,no判定に使用 */
enum YesNoRetry {
	YES = 1,
	NO = 2,
	RETRY = 3
};	

/* 新規会員登録時使用 */
enum RegisterStatus {
	REGISTER_START = 0,
	REGISTER_NAME = 1,
	REGISTER_CLASS = 2,
	REGISTER_AGE = 3,
	REGISTER_GENDER = 4,
	REGISTER_NOTE = 5,
	REGISTER_CONFIRM = 6,
	REGISTER_COMPLETED = 7,
	REGISTER_ERROR_MAX_MEMBER = 8,
        REGISTER_ERROR_UNKNOWN = 9,
	REGISTER_ERROR_MEMORY = 10,
	REGISTER_SUCCESSED = 11
};

/* 会員情報編集時使用 */
enum EditStatus {
	EDIT_START = 0,
	EDIT_CANCEL = 1,
	EDIT_SELECT_MEMBER = 2,
	EDIT_MEMBER_NOT_FOUND = 3,
	EDIT_ITEM = 4,
	EDIT_CONFIRM = 5,
	EDIT_COMPLETED = 6,
	EDIT_NO_MEMBER = 7,
	EDIT_MEMBER_IS_DELETED = 8
};

/* 会員情報削除時使用 */
enum DeleteStatus {
	DELETE_START = 0,
	DELETE_CANCEL = 1,
	DELETE_SELECT_MEMBER = 2,
	DELETE_MEMBER_NOT_FOUND = 3,
	DELETE_CONFIRM = 4,
	DELETE_COMPLETED = 5,
	DELETE_NO_MEMBER = 6,
	DELETE_IS_ALREADY_DELETED = 7
};

/* 会員情報閲覧時使用 */
enum ViewStatus {
	VIEW_START = 0,
	VIEW_WAIT = 1,
	VIEW_EXIT = 2,
	VIEW_NO_MEMBER = 3
};


/*>>>>> 関数のプロトタイプ宣言と各ファイルの説明 <<<<<*/
/*===== tool.c =====*/
/* ファイル説明:
 * よく使う便利な関数をまとめている 
 */

/*
 * 使い方: - ユーザの入力を引数とした配列に渡す
 * 引数: char *line  - ユーザ入力を受け取りたい配列のポインタ
 *       int size  - 第一引数の配列の大きさ(あらかじめsizeof(line)で求めてこの引数として使用する) 
 * 戻り値: true  - ユーザ入力を配列に渡せた場合
 *         false  - ユーザ入力を配列に渡せなかった場合
 */
bool get_cmd(char *line, int size);

/*
 * 使い方: - 指定した配列に数字があれば読み取って、引数のint型ポインタに数字を渡す。
 * 引数: char *line  - ユーザ入力を受け取りたい配列のポインタ
 *       int *cmd  - 数字入力を受け取りたい変数のポインタ
 * 戻り値: true  - 正常に数字を渡せた場合
 *         false  - 数字を渡せなかった場合
 */
bool check_line_is_num(char *line, int *cmd );

/*
 * 使い方:  - 第1引数の配列に文字列があれば読み取って、第2引数の配列に渡す。(入力文字列先頭の空白を削除する)
 * 引数: char *line  - ユーザ入力を受け取りたい配列のポインタ
 * 	 char *cmd_line  - 読み取れた文字列を渡す配列のポインタ
 * 戻り値: true  - 正常に文字列を渡せた場合
 *         false  - 文字列を渡せなかった場合
 */
bool check_line_is_str(char *line, char *cmd_line);


/*
 * 使い方: - 第一引数の配列の一文字目を判定(y,Y,n,N,その他)してYES,NO,RETRYを返す
 * 引数: char *cmd_line - 判定したい配列のポインタ 
 * 戻り値: enum YesNoRetry  - ユーザ入力に対する分岐に使用
 */
enum YesNoRetry check_yes_or_no(char *cmd_line);

/*
 * 使い方: - fgetの後に入力バッファに入力が残っていた場合使用する。
 * 引数: なし
 * 戻り値: なし
 */
void clear_input_buffer (void);

/*===== ui.c =====*/
/* ファイル説明:
 * ユーザに表示するUIを扱う関数をまとめる
 */

/*
 * 使い方: - プログラム起動後に使用し、ファイル読み込み時のUIを表示する
 * 引数: なし
 * 戻り値: なし
 */
void start_message_ui();  

/*
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
 * 使い方: - すべての会員情報を表示する
 * 引数: struct Member *head  - 会員番号1番のアドレス
 * 戻り値: enum SearchMemberStatus  - 会員が見つかったかを示す
 */        
enum SearchMemberStatus display_member(struct Member *head);

/*
 * 使い方: - 指定した会員情報を表示する
 * 引数: struct Member *temp  - 表示したい会員のアドレス
 * 戻り値: enum SearchMemberStatus  - 会員が見つかったかを示す
 */        
enum SearchMemberStatus display_solo_member(struct Member *temp);

/*
 * 使い方: - 新規会員登録時に使用するUI
 * 引数: enum RegisterStatus  - 会員登録進行状況 
 *       struct Member *temp  - 登録するデータの仮置き場のアドレス
 * 戻り値: enum CancelCheck
 */
enum CancelCheck register_ui(enum RegisterStatus status, struct Member *temp);

/*
 * 使い方: - 会員情報編集時に使用するUI
 * 引数: enum EditStatus  - 会員情報編集進行状況
 *       struct Member *temp  - 編集する予定の新規会員情報を一時保存するアドレス
 * 戻り値: enum CancelCheck
 */
enum CancelCheck edit_ui(enum EditStatus status, struct Member *temp);

/*
 * 使い方: - 会員情報削除時に使用するUI
 * 引数: enum DeleteStatus  - 会員情報編集進行状況
 *       struct Member *temp  - 削除する予定の新規会員情報を一時保存するアドレス
 * 戻り値: enum CancelCheck
 */
enum CancelCheck delete_ui(enum DeleteStatus status, struct Member *temp);  

/*
 * 使い方: - 会員情報閲覧時に使用するUI
 * 引数: struct Member *status  - メッセージの種類
 * 戻り値: なし
 */
void view_ui(enum ViewStatus status);  

/*
 * 使い方: - プログラム終了時のメッセージを表示する
 * 引数: なし
 * 戻り値: なし
 */
void quit_message_ui();  

/*
 * 使い方: - ファイル書き込み時に使用するUI
 * 引数: enum WriteFileStatus  - ファイル書き込み進行状況
 * 戻り値: なし
 */
void write_file_ui(enum WriteFileStatus status);

/*===== member.c =====*/
/* ファイル説明:
 * 会員情報の検索、変更を行う関数をまとめる 
 */


/*
 * 使い方: - 新規会員番号を得て、第2引数のtemp->member_numに代入する
 * 引数: struct Member *head  - 会員番号1番のアドレス
 *       struct Member *temp  - 新規会員情報を一時的に保管するアドレス
 * 戻り値: true  - 新規会員番号を渡した場合
 *         false  - 会員数が上限で会員番号を渡せなかった場合
 */
bool register_get_new_member_num(struct Member *head, struct Member *temp);	


/*
 * 使い方: - 第2引数で指定した会員番号の会員情報があるアドレスを第3引数に渡す
 * 引数: struct Member *head  - 会員番号1番のアドレス
 *       int member_num  - アドレスを知りたい会員の番号
 *       struct Member **member_address  - 見つけた会員のアドレスを渡す先
 * 戻り値: enum SearchMemberStatus
 *         
 */
enum SearchMemberStatus search_member_address(struct Member *head, int member_num, struct Member **member_address);  


/*
 * 使い方: - 登録実行時に使用する
 * 引数: struct Member **head  - 会員番号1番のアドレスのポインタ
 *       struct Member *temp  - 登録したい会員情報のアドレス
 * 戻り値: enum RegisterStatus  - 登録進行状況
 *         
 */
enum RegisterStatus register_execute(struct Member **head, struct Member *temp);


/*
 * 使い方: - 最後尾の会員のアドレスを引数に渡す
 * 引数: struct Member *head  - 会員番号1番のアドレス
 *       struct Member **tail  - 最後尾の会員のアドレスのポインタ 
 * 戻り値: SEARCHTAIL_SUCCESSED  - 成功した場合
 *         SEARCHTAIL_FAILED  - 失敗した場合
 *         SEARCHTAIL_NO_MEMBER  - 会員が見つからなかった場合
 */
enum SearchTailStatus search_tail(struct Member *head, struct Member **tail);

/*
 * 使い方: - 会員情報削除実行時に使用する
 * 引数: struct Member *delete_member_address  - 削除したい会員情報のアドレス
 * 戻り値: DeleteStatus
 */
enum DeleteStatus delete_execute(struct Member *delete_member_address);

/*
 * 使い方: - プログラム終了前にすべてのメモリを解放する
 * 引数: struct member *head  - 会員番号1番のアドレス
 * 戻り値: なし
 */
void free_all_memory(struct Member *head);

/*===== file_io.c =====*/
/* ファイル説明:
 * ファイルに読み書きする関数をまとめる
 */

/*
 * 使い方: -
 * 引数: struct member **head  - 会員番号1番のアドレスのポインタ
 * 	 int *loaded_count  - 読み込んだデータ件数のポインタ
 * 戻り値: enum ReadFileStatus
 */
enum ReadFileStatus read_file(struct Member **head, int *loaded_count);

/*
 * 使い方: - ファイル書き込みを実行する関数(tsv形式)
 * 引数: struct member *head  - 会員番号1番のアドレス
 * 戻り値: true  - 成功した場合
 *         false  - 失敗した場合
 */
bool write_file(struct Member *head);

#endif
