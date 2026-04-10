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
	bool is_male;  /* 男性なら1 */
	bool is_female;  /* 女性なら1 */
	bool is_currently_registering;  /* 会員登録中なら0,退会済みなら1 */
	char note[4096];  /* 備考 */
};

/*>>> グローバル変数の定義 <<<*/
char line[4096] = {0};  /* ユーザ入力用、名前や備考入力にも使うので長め */
struct Member read_data[MAX_MEMBER] = {0};  /* ファイルから読み出されたデータを保管する配列 */
int loaded_count = -1;  /* ファイルから読み出された会員数を数える変数、初期値は-1とする */

/*>>> ファイルが空かチェックする関数 <<<*/
int  is_file_empty(FILE *fp) {
/*===== 編集中 =====*/
}

/*>>> ファイル読み出し関数 <<<*/
void load_func() {
	FILE* fp = fopen("member_save.dat", "rb");
	
	/* もしファイルを読み取れなかったら */
	if (fp == NULL) {
		printf("セーブファイルが見つかりませんでした。新規セーブファイルを作成します。\n");
		fp = freopen("member_save.dat", "wb", fp);
		loaded_count = 0;  /* 読み出す会員数は0人で確定 */
		fclose(fp);
		return;
	}
	
	/* 空のファイルかチェックする */
	if (is_file_empty(fp) == 1) {  /*=== ファイルが空かチェックする関数、1なら空ファイル ===*/
		printf("セーブファイルが空でした。\n");
		loaded_count = 0;  /* 読み出す会員数は0人で確定 */
		fclose(fp);
		return;
	}
	
	/* ファイルの読み出し */
	/*===== 編集中 =====*/	

}
/*>>> 会員情報登録時の未入力チェック関数 <<<*/

/*
 * グローバル変数lineにユーザ入力を入れることに注意
 */

int regi_cancel_func() {
	memset(line, 0, sizeof(line));  /* lineを0で初期化 */
	if (fgets(line, sizeof(line), stdin) == NULL || line[0] == '\n') {
		printf("登録を中断します。\n");
		return -1;
	}
	return 0;
}	

/*>>> 会員情報登録時の数字読み取り用関数 <<<*/

/*
 * regi_cancel_funcのあとなど、fgetsでグローバル変数lineにユーザ入力を入れてから使用する
 */

int regi_read_num(int *out_num) {
	/* 数字が読み取れなかった場合 */
	if (sscanf(line, " %d", out_num) != 1) { 
		printf("入力が読み取れませんでした。やり直してください。\n");
		return -1;
	}
	return 0;
}


/*>>> 会員情報登録関数 <<<*/
void regi_func() {

	/*
	 * 入力された新規会員情報は一時的に保存するための変数に代入し、この関数の最後でまとめて本データに入力する処理をする
	 */
	
	/*>>> 新規会員情報を一時的に保存する変数 <<<*/
	int new_member_num = loaded_count + 1;  /* 会員番号 */
	char new_name[1024] = {0};  /* 氏名 */
	int new_class;  /* クラス（1~5級） */
	int new_age;  /* 年齢 */
	int gender_num = -1;  /* ユーザの性別入力時の数字入力を保存し、new_is_male, new_is_femaleを操作する */
	bool new_is_male = -1;  /* 男性なら1 */
	bool new_is_female = -1;  /* 女性なら1 */
	char new_note[4096] = {0};  /* 備考 */
	
	printf("新しい会員を登録します。必要な情報を確認してください。未入力でEnterを押すと登録を中断します。\n");
	printf("必要な情報：1.名前 2.クラス 3.年齢 4.性別 5.備考\n");
	
	/*>>> 1.名前の入力 <<<*/
	while (1) {
		printf("1.名前を入力してください。\n");	

		/* 名前の入力がない場合 */
		if (regi_cancel_func() == -1) {
			return;  /* メニューに戻る */
		}
		/* 文字列が読み取れなかった場合 */
		if (sscanf(line, " %s", new_name) != 1) {  /* グローバル変数lineにはregi_cancel_funcでユーザ入力が入っている */
			printf("入力が読み取れませんでした。やり直してください。\n");
			continue;
		}
		printf("名前：%s\n", new_name);  /* 入力した名前の表示 */
		break;
	}
	
	/*>>> 2.クラスの入力 <<<*/
	while (1) {
		printf("2.クラスを入力してください。(1~5級)\n");
		/* 入力がなかったとき */
		if (regi_cancel_func() == -1) {
			return;
		}
		/* 数字が読み取れなかった場合 */
		if (regi_read_num(&new_class) == -1) {	
			continue;
		}
		
		/* 数字がクラスの範囲外のとき */
		if (new_class < 1 || new_class > 5) {
			printf("クラスは1~5級までです。やり直してください。\n");
			continue;
		}
		printf("クラス：%d級\n", new_class);
		break;
	}
	
	/*>>> 3.年齢の入力 <<<*/
	while (1) {
		printf("3.年齢を入力してください。\n");
		/* 入力がなかったとき */
		if (regi_cancel_func() == -1) {
			return;
		}
		/* 数字が読み取れなかった場合 */
		if (regi_read_num(&new_age) == -1) {	
			continue;
		}
		printf("年齢：%d\n", new_age);
		break;
	}
	
	/*>>> 4.性別の入力 <<<*/
	while (1) {
		printf("4.性別を入力してください。\n"
			"1) 男性\n"
	      		"2) 女性\n"
			"3) その他\n"
		      );
		/* 入力がなかったとき */
		if (regi_cancel_func() == -1) {
			return;
		}
		/* 数字が読み取れなかった場合 */
		if (regi_read_num(&new_gender) == -1) {	
			continue;
		}

		/*>>> 性別フラグの操作 <<<*/
		/* 男性のとき */
		if (new_gender == 1) {  
			new_is_male = 1;
			new_is_famale = 0;
			printf("性別：男性\n");
			break;
		}
		/* 女性のとき */
		else if (new_gender == 2) {
			new_is_male = 0;
			new_is_female = 1;
			printf("性別：女性\n");
			break;
		}
		/* その他の性別のとき */
		else if (new_gender == 3) {
			new_is_male = 0;
			new_is_female = 0;
			printf("性別：その他\n");
			break;
		}
		else {
			printf("不適切な入力です。やり直してください。\n");
			continue;
		}
	}

	/*>>> 5.備考の入力 <<<*/
	while (1) {
		printf("5.備考を入力してください。未入力でも次に進みます。\n");
		if (fgets(new_note, sizeof(new_note), stdin) == NULL || line[0] == '\n') {
			printf("備考を入力せずに次に進みます。\n");
			break;			
		}
		printf("備考：%s", new_note);
		break;
	}

	/*>>> 入力情報の表示と本データへの入力 <<<*/
	while (1) {	
		printf("以下の内容で新規会員情報を登録してもよろしいですか？[y/n]\n");
		/*>>> 会員情報を表示 <<<*/
		printf("会員番号：%d\n"
			"名前：%s\n"
			"クラス：%d\n"
			"年齢：%d\n"
			, new_member_num, new_name, new_class, new_age);
		/* 性別表示 */
		if (new_gender == 1) {
			printf("性別：男性\n");
		}
		else if (new_gender == 2) {
			printf("性別：女性\n");
		}
		else if (new_gender == 3) {
			printf("性別：その他\n");
		}
		/* 備考を表示 */
		printf("備考：%s", new_note);  /* 備考入力で改行が入っているので\nは不要 */
		
		/*>>> 新規会員情報登録の決定 <<<*/
		memset(line, 0, sizeof(line));
		char cmd_regi[128] = {0};
		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("入力がありませんでした。やり直してください。\n");
			continue;
		}
		if (sscanf(line, " %s", cmd_regi) != NULL) {
			if (cmd_regi[0] == 'y' || cmd_regi[0] == 'Y') {
				/* 会員情報を本データに書き込む */
				/*===== 編集中 =====*/
				printf("会員情報を登録しました。\n");
				break;
			else if (cmd_regi[0] = 'n' || cmd_regi[0] == 'N') {
				printf("会員情報の登録を中止します。\n");
				return;
			}
		}
		else {
			printf("不適切な入力です。やり直してください。\n");
			continue;
		}
	}
}

/*>>> 会員情報を編集する関数 <<<*/
void edit_func() {
	printf("会員情報編集プログラムを起動します。\n");
	printf("誰の情報を編集しますか？\n");
	/* 現在退会していない会員番号と名前をforループで表示する */
	/*===== 編集中 =====*/
}
/*>>> 会員情報を削除（退会）する関数 <<<*/
void dele_func() {
	printf("退会する会員を選んでください。\n");
	/* 現在入会中の会員番号と名前、クラス、年齢をforループで表示する */
	/*===== 編集中 =====*/

	/* ユーザ入力と入力検証 */

	printf("~さんを退会させます。再入会する場合は新規登録が必要になります。よろしいですか？\n");  /*===== 変数で~を置き換える =====*/
	/* ユーザ入力と入力検証 */
	/* 退会する場合は名前を伏せ字に置き換え、備考を削除する */
	printf("退会処理が完了しました。メニューに戻ります。\n");
	printf("退会処理を中止します。メニューに戻ります。\n");
}

/*>>> 会員情報を閲覧する関数 <<<*/
void view_func() {
	printf("現在入会中の会員情報を表示します。\n");
	/* 現在入会中の会員番号、名前、クラス、年齢、性別、備考をforループで表示する */
	/* Enter入力でメニューに戻る */
	
}


int main() {
/*>>> ファイル読み出し <<<*/
	load_func();  /*=== ファイル読み出し用関数を起動 ===*/

/*>>> プログラム起動時のプロンプト <<<*/
	printf("会員情報プログラムを起動しました。\n");

/*>>> メニュー画面 <<<*/
	/* 繰り返し操作できるようにwhileループさせる */
	while (1) {
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
			regi_func();  /*=== 会員登録用関数を起動 ===*/
			continue;
		}
		else if (cmd_num == 2) {
			printf("会員情報編集プログラムを起動します。\n");
			edit_func();  /*=== 会員編集用関数を起動 ===*/
			continue;
		}
		else if (cmd_num == 3) {
			printf("会員情報削除プログラムを起動します。\n");
			dele_func();  /*=== 会員削除用関数を起動 ===*/
			continue;
		}
		else if (cmd_num == 4) {
			printf("会員情報閲覧プログラムを起動します。\n");
			view_func();  /*=== 会員閲覧用関数を起動 ===*/
			continue;
		}
		else {  /* その他の数字が入力されたとき */
			printf("不適切な入力です。やり直してください。\n");
			continue;
		}
	}  /* whileループの終わり */
	return 0;
}  /* main関数の終わり */







	
	

