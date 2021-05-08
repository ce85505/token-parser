#ifndef PARSER_H
#define PARSER_H

#define MAX_STR_SIZE 128

struct token {
	char type[MAX_STR_SIZE];
	int inst;
	int line;
};

struct parser {
	struct token *data;
	int num, pos;
};

struct node {
	struct node *middle;
	struct node *left;
	struct node *right;
	struct token token, token1;
	char label[MAX_STR_SIZE];
	int len, num;
};

struct node *parser (struct parser *p);


#endif