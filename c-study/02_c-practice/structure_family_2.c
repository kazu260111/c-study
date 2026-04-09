#include <stdio.h>
#include <stdbool.h>
#include <string.h>  /* strcmp用 */
#include <strings.h>  /* strcasecmp用 */
/*
 * === 改善版 ===
 * 登録した家族情報を閲覧できるプログラム2
 * Alice, Bob, Chris, Pole, Polaの家族の情報と関係性をユーザの入力で調べることができる
 */

#define MAX_SIBLING 5  /*--- 最大きょうだい数を変えたい場合ここを変える ---*/

struct Family {
	char name[128];
	int age;
	bool is_male;  /* 男性なら1 */
	bool is_female;  /* 女性なら1 */
	struct Family *father;  /* 父親 */
	struct Family *mother;  /* 母親 */
	struct Family *partner;  /* 配偶者 */	
	/*=== 変更点 ===*/
	struct Family *sibling[MAX_SIBLING];  /* きょうだいを配列にすることで人数の制限を簡単に増やせるようにした */
	char preference[256];  /* 好きなもの */
};

/*>>> 家族情報を表示する関数の定義 <<<*/
void print_status(struct Family *family_mem) {	
	printf("\n");  /* 改行 */

	/* 名前と年齢の表示 */
	printf("名前：%s\n", family_mem->name);
	printf("年齢：%d\n", family_mem->age);
	
	/* 性別の表示 */
	if (family_mem->is_male == 1 && family_mem->is_female == 0) {
		printf("性別：男性\n");
	}
	else if (family_mem->is_male == 0 && family_mem->is_female == 1) {
		printf("性別：女性\n");
	}
	else {
		printf("性別：その他\n");
	}

	/* 父親の表示 */
	if (family_mem->father != NULL) {
		printf("父親：%s\n", family_mem->father->name);
	}
	/* 母親の表示 */
	if (family_mem->mother != NULL) {
		printf("母親：%s\n", family_mem->mother->name);
	}

	/* 配偶者の表示 */
	if (family_mem->partner != NULL) {
		printf("配偶者：%s\n", family_mem->partner->name);
	}
	
	/* きょうだいの表示 */
	/*=== 変更点 ===*/
	/* きょうだいを配列にしたのでforループで順に表示するようにした */
	for (int i = 0; (i < MAX_SIBLING) && (family_mem->sibling[i] != NULL); ++i) {  
		/* メモ：構造体の変数のメンバは定義されないときNULLに初期化される */
		/* 先に i < MAX_SIBLING を評価することでsiblingの配列外を評価しないようにする */
		printf("きょうだい：%s\n", family_mem->sibling[i]->name);
	}
	/* 注意：sibling配列は途中でNULLがあるとその後の人が読めないので、きょうだいを編集するときは小さい方に詰める */
	
	/* 好きなものの表示 */
	if (family_mem->preference[0] != '\0') {  /* 好きなものが入力されているなら(空文字でないなら) */
		printf("好きなもの：%s\n", family_mem->preference);
	}
	else {
		printf("好きなもの：非公開\n");
	}

	printf("\n");  /* 改行 */
	return;
}


int main(void) {
/*>>> 変数の定義 <<<*/
	/*=== 変更点 ===*/
	/* きょうだいを配列にしたので該当部分を配列{}にした */
	/* 両親から定義する */
	struct Family pole = {"Pole", 45, 1, 0, NULL, NULL, NULL, {NULL}, "ペンギン"};  /* 配偶者はまだ定義されていないのであとで代入する */
	struct Family pola = {"Pola", 40, 0, 1, NULL, NULL, &pole, {NULL}, "絵画"};
	pole.partner = &pola;

	/* 子供の定義 */
	struct Family alice = {"Alice", 15, 0, 1, &pole, &pola, NULL, {NULL}, "りんご"};  /* きょうだいはまだ定義されていないのであとで代入する(二人入れる) */
        struct Family bob = {"Bob", 12, 1, 0, &pole, &pola, NULL, {NULL}, "野球"}; 	/* きょうだいはまだ定義されていないのであとで代入する（ひとり入れる）*/
        struct Family chris = {"Chris", 10, 1, 0, &pole, &pola, NULL, {NULL}, "チョコレート"};
	
	/*=== 変更点 ===*/
	/* 子供のきょうだいは初期化時には全員NULLにして、あとからきょうだいを全員入力することにした */
	alice.sibling[0] = &bob;
	alice.sibling[1] = &chris;

	bob.sibling[0] = &alice;
	bob.sibling[1] = &chris;

	chris.sibling[0] = &alice;
	chris.sibling[1] = &bob;

	/*--- 1.家族を追加したい場合ここに追加 (1/2) ---*/


	/*=== 変更点 ===*/
	/* ポインタの配列 */
	struct Family *families[] = {&alice, &bob, &chris, &pole, &pola};  /*--- 2.家族を追加したい場合はこの配列に追加 (2/2) ---*/
	/* 家族の人数を自動計算 */
	int family_count = sizeof(families) / sizeof(families[0]);
	
	/* 連続して情報を確認できるようwhileループしている。通常終了する場合はbreakをする */
	while (1) { 
/*>>> 変数の初期化 <<<*/
		/* ループするので毎回使う変数をリセットしておく */
		char line[128] = {0};  /* ユーザ入力受付用 */
		char cmd[128] = {0};  /* ユーザ入力の文字列を入れる変数 */
		int family_num = 0;  /* ユーザ入力の数字を入れる変数 */

/*>>> プロンプトと入力受付 <<<*/
		printf("誰の情報を確認しますか？\n");
		/*=== 変更点 ===*/
		/* forループを使うことで家族が増えても対応できるようにした */
		for (int i = 0; i < family_count; ++i) {
			printf("%d) %s\n", i + 1, families[i]->name);
		}
		printf("q) やめる\n");		
		
		/* 入力がなかった場合終了する */
		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("入力がありませんでした。プログラムを終了します。\n");
			return 1;
		}
	
		/* q,Qが入力されたら終了する */
		if (sscanf(line, " %s", cmd) != 0) {  /* 一度cmdに入れてからcmd[0]をチェックする(空白入力をスキップしたいので) */
			if (cmd[0] == 'q' || cmd[0] == 'Q') {
				printf("終了コマンドが選択されました。プログラムを終了します。\n");
				break;
			}
		}
		/*
		 *  注意：もしqまたはQで始まる人名が入力された場合ここで終了してしまう。
		 *  現在の仕様では数字を入れることになっているので問題ないが、人名を入れられる仕様にしたい場合処理順を変える必要がある。
		 */
		
		
		/* 数字の読み取りを試行する */
		sscanf(line, " %d", &family_num); 
		
		/*
		 * ここまでで数字が入力されたならfamily_numにその数字が入り、
		 * 文字が入力されたならcmdにその文字列が入っている。
		 */

/*=== 変更点 ===*/
/*>>> ユーザ入力に対応する家族情報を検索する <<<*/
		/* forループにすることで、家族が増えたときもルーチンを変えずに済むようにする */
		bool found = false;  /* メモ：stdboolをインクルードしているとtrue,falseが使える(True, Falseのように大文字にしない) */
		for (int i = 0; i < family_count; ++i) {
			if (family_num == i + 1 || strcasecmp(cmd, families[i]->name) == 0) {
				print_status(families[i]);
				found = true;
				break;
			}
		}
		/* 配列にそのままユーザ入力を入れると危険なのでforループで家族情報を検索する形になった */

		/* 情報が見つからなかったとき */
		if (!found) {
			printf("情報が見つかりませんでした。入力をやり直してください。\n");
			continue;  /* whileループの最初に戻る */
		}
	}  /* whileループの終わり */
	return 0;
}

/*
 * 今後の予定：
 * 次回はユーザが自由に家族（メンバー）を追加できるプログラムを作成したい。
 * 起動時にはファイルからメンバーを読み込むことで、普通のアプリケーションのようにメンバーを編集できるようにする予定。
 */


/* 
 * 参考：
 * 以下は前バージョンで使っていたルーチンの一部
 * forループに比べると非常に冗長な上、家族が増えたときの作業量が多い。
 */
#if 0
/*>>> 関数の実行による家族情報表示 <<<*/
		/* family_numによって引数を変えて関数を実行する */
		if (family_num == 1 || strcmp(cmd, "Alice") == 0 || strcmp(cmd, "alice") == 0) {
			print_status(&alice);
		}
		else if (family_num == 2 || strcmp(cmd, "Bob") == 0 || strcmp(cmd, "bob") == 0) {
			print_status(&bob);
		}
		else if (family_num == 3 || strcmp(cmd, "Chris") == 0 || strcmp(cmd, "chris") == 0) {
			print_status(&chris);
		}
		else if (family_num == 4 || strcmp(cmd, "Pole") == 0 || strcmp(cmd, "pole") == 0) {
			print_status(&pole);
		}
		else if (family_num == 5 || strcmp(cmd, "Pola") == 0 || strcmp(cmd, "pola") == 0) {
			print_status(&pola);
		}
		else {
			printf("不適切な入力です。プログラムを終了します。\n");
			return 1;
		}
	}
	return 0;
}
#endif
