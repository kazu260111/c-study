
enum SearchMemberStatus search_member_address(struct Member *head, int member_num, struct Member **member_address);  
int register_execute(struct Member *head, struct Member *temp);
int delete_execute(struct Member *temp);
void free_all_memory(struct member *head);
