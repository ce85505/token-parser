#ifndef TREE_H
#define TREE_H

#include "scanner.h"

struct node *add_node (const char *data);
void delete_tree (struct node *root);
void printPreorder (struct node *root, int level, const char *name);

#endif