#include "member.h"

enum ReadFileStatus read_file(struct Member **head, int *loaded_count) {
	FILE *fp = fopen("member.tsv", "r");
	if (fp == NULL) {
		return READ_FAILED;
	}
	/*>>> 1行目の読み込み <<<*/
	char line[8192] = {0};
	if (fgets(line, sizeof(line), fp) == NULL) {
		fclose(fp);
		return READ_EMPTY_FILE;
	}
	struct Member *first_member_address = calloc(1, sizeof(struct Member));
	if (first_member_address == NULL) {
		fclose(fp);
		return READ_ERROR_MEMORY;
	}
	/* headに最初の会員のアドレスを入れる */
	*head = first_member_address;
	*loaded_count = 1;
	/* 確保したアドレスにデータを入れる */
	int status = scanf(line, "%d\t%1023[^\t]\t%d\t%d%s\t%d\t%4098[^\n]", 
			&first_member_address->member_num,
			first_member_address->name,
			&first_member_address->class_num,
			&first_member_address->age,
			first_member_address->gender,
			&first_member_address->is_deleted_account,
			first_member_address->note
	 		    );
	if (status != 7) {
		read_file_ui(READ_ERROR_ABNORMAL_DATA, 0);
		free_all_memory(*head);
		fclose(fp);
		exit(1);
	}
	/* nextにアドレスを入れるために、以降のループ中で一つ前のアドレスをこの変数に入れる */
	struct Member *back_address = *head;
	/*>>> 2行目以降の読み込み <<<*/
	while (1) {
		status = 0;
		memset(line, 0, sizeof(line));
		if (fgets(line, sizeof(line), fp) == NULL) {
			fclose(fp);
			return READ_SUCCESSED;
		}
		struct Member *member_address = calloc(1, sizeof(struct Member));
		if (member_address == NULL) {
			free_all_memory(*head);
			fclose(fp);
			return READ_ERROR_MEMORY;
		}
		/* 確保したアドレスにデータを入れる */
		status = sscanf(line, "%d\t%1023[^\t]\t%d\t%d\t%s\t%d\t%4095[^\n]", 
				&member_address->member_num,
				member_address->name,
				&member_address->class_num,
				&member_address->age,
				member_address->gender,
				(int *)&member_address->is_deleted_account,
				member_address->note
		 		    );
		if (status != 7) {
			read_file_ui(READ_ERROR_ABNORMAL_DATA, 0);
			free_all_memory(*head);
			fclose(fp);
			exit(1);
		}
		/* 一つ前のアドレスをNULLから今のアドレスに変更 */
		back_address->next = member_address;	
		back_address = member_address;
		/* 読み込み件数のカウントを増やす */
		++*loaded_count;
		if (*loaded_count >= MAX_MEMBER) {
			read_file_ui(READ_MAX_MEMBER, 0);
			fclose(fp);
			return READ_SUCCESSED;
		}

	}
	return READ_FAILED;
}

bool write_file(struct Member *head) {
	FILE *fp = fopen("member.tsv", "w");
	if (fp == NULL) {
		return false;
	}
	if (head == NULL) {
		fclose(fp);
		return false;
	}
	struct Member *current_member = head;
	while (current_member != NULL) {
		fprintf(fp, "%d\t%s\t%d\t%d\t%s\t%d\t%s\n", 
			current_member->member_num,
			current_member->name,
			current_member->class_num,
			current_member->age,
			current_member->gender,
			(int)current_member->is_deleted_account,
			current_member->note
			);
		current_member = current_member->next;
	}
	fclose(fp);
	return true;
}

