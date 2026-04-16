bool register_get_new_member_num(struct Member *head, struct Member *temp) {
	struct Member *tail = NULL;
	enum SearchTailStatus search_status = search_tail(head, &tail);
	switch (search_status) {
		case SEARCHTAIL_NO_MEMBER:
			temp->member_num = 1;
			return true;
		case SEARCHTAIL_SUCCESSED:
			temp->member_num = tail->member_num + 1;
			return true;
		case SEARCHTAIL_FAILED:
			return false;
	}
	return false;
}


enum SearchTailStatus search_tail(struct Member *head, struct Member **tail) {
	/* 会員がいない場合NO_MEMBERを返す */
	if (head == NULL) {
		return SEARCHTAIL_NO_MEMBER;
	}
	struct Member *current_address = head;
	while (current_address->next != NULL) {
		current_address = current_address->next;
	}
	*tail = current_address;
	return SEARCHTAIL_SUCCESSED;
}

enum SearchMemberStatus search_member_address(struct Member *head, int member_num, struct Member **member_address) {
	if (head == NULL) {
		return NOT_FOUND;
	}
	struct Member current_address = head;
       	int current_member_num = current_address->member_num;
       	while (current_member_num != member_num) {
		if (current_address->next == NULL) {
			return NOT_FOUND;
		}
	        current_address = current_address->next;
	        current_member_num = current_address->member_num;
	}
	if (current_address->is_deleted_account) {
		return IS_DELETED;
	}
	*member_address = current_address;
	return FOUND;
}

enum RegisterStatus register_execute(struct Member **head, struct Member *temp) {
	struct Member *new_address = calloc(1, sizeof(struct Member));
	if (new_address == NULL) {
		return REGISTER_ERROR_MEMORY;
	}
	struct Member *tail = NULL;
	enum SearchTailStatus search_status = search_tail(*head, &tail);
	switch (search_status) {
		case SEARCHTAIL_SUCCESSED:
			/* 最後尾の会員のnextに新しい会員のアドレスを入れる */
			tail->next = new_address;
			break;
		case SEARCHTAIL_NO_MEMBER:
			/* 一人目の会員なのでheadを更新 */
			*head = new_address 
			break;
		case SEARCHTAIL_FAILED:
			return REGISTER_ERROR_UNKNOWN;
	}
	/* tempにあるデータを確保したメモリに入れる */
	*new_address = *temp;
	return REGISTER_SUCCESSED;
}

enum DeleteStatus delete_execute(struct Member *delete_member_address) {
	memset(delete_member_address->name, 0, sizeof(delete_member_address->name));
	strcpy(delete_member_address->name, "******");
	memset(delete_member_address->note, 0, sizeof(delete_member_address->note));	
}

void free_all_memory(struct member *head) {
	/* メモリを使っていないとき */
	if (head == NULL) {
		return;
	}

	struct Member *current_address = head;
	while (current_address->next != NULL) {
		/* current_address->nextをnext_addressに退避 */
		struct Member *next_address = current_address->next;
		free(current_address);
		current_address = next_address;
	}
	free(current_address);
	return;
}
		

	



