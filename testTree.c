#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "testTree.h"
#include "scanner.h"
#include "parser.h"

struct node *add_node (const char *data)
{
	struct node *retn = (struct node *)calloc(sizeof(struct node), 1);

	retn->num = 1;
	retn->left = retn->right = NULL;
	strncpy(retn->label, data, 128);
	
	return retn;
}

void delete_tree (struct node *root)
{
	if (!root)
		return;
	
	delete_tree(root->left);
	delete_tree(root->right);
}

void print_node (struct node *root, int level, int fd) /* actually print */
{
	char *out = (char *)calloc(MAX_STR_SIZE, root->num + 2);
	int lenb;
	
	if (level)
		snprintf(out, MAX_STR_SIZE, "%*c", level * 2, ' ');

	strcat(out, root->label);
	if (root->token.type[0]) {
		strcat(out, ": ");
		strcat(out, root->token.type);
	}
	if (root->token1.type[0]) {
		strcat(out, ", ");
		strcat(out, root->token1.type);
	}
	
	strcat(out, "\n");
	lenb = (int)strlen(out);
	
	if ((int)write(fd, out, lenb) != lenb)
		perror("write");
		
	printf("%s", out);
	free(out);
}

void wrapper (struct node *root, int level, const char *name, const char *ext, 
	void (*fn)(struct node *, int, int)) /* print one node wrapper for any traversal */
{
	int fd;
	char buf[128];
	snprintf(buf, sizeof(buf), "%s.%s", name, ext);
	if ((fd = open(buf, O_RDWR|O_CREAT|O_TRUNC, 0777)) < 0) {
		perror("open");
	} else {
		fn(root, level, fd);
	}
	close(fd);
}
		
void _printPreorder (struct node *root, int level, int fd) /* root, left, right */
{
	if (root == NULL)
		return;
	print_node(root, level, fd);
	_printPreorder(root->left, level+1, fd);
	_printPreorder(root->middle, level+1, fd);
	_printPreorder(root->right, level+1, fd);
}
void printPreorder (struct node *root, int level, const char *name) /* root, left, right */
{
	wrapper(root, level, name, "preorder", _printPreorder);
}
