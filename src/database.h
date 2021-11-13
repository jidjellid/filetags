#ifndef __DATABASE__H__

#define __DATABASE__H__

void init_db();

int insert_db(char const *parent_category, char const *child_category);

int delete_db(char const *category);

char *from_child_to_parent(char const *category);

char *from_parent_to_children(char const *category);

#endif