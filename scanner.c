#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

int get_instances (struct parser *np, int index, char *buf)
{
	int count = 0;
	int lens = (int)strlen(buf);
	int i;
	
	if (index == 0)
		return 0;
	
	for (i = 0; i < index - 1; i++) {
		if ((int)strlen(np->data[i].type) == lens &&
		    !strncmp(buf, np->data[i].type, lens))
			count++;
	}
	
	return count;
}

struct parser *init_parser (int file)
{
	int rr = 0, tot = 0;
	char buf[MAX_STR_SIZE];
	char *dst = calloc(1, 12000000), *ptr0 = dst, *ptr1 = dst;
		
	while ((rr = (int)read(file, dst + tot, 100000)) > 0) {
		if (rr < 0) {
			perror("read");
			free(dst);
			return NULL;
		}
		tot += rr;
	}
	
	if (tot <= 0) {
		printf("error: read %d bytes\n", tot);
		free(dst);
		return NULL;
	}
	
	struct parser *np = calloc(1, sizeof(struct parser));
	
	int counter = 0;
	while (*ptr1) {
		memset(buf, 0, sizeof(buf));
		while (*ptr1 && !isspace(*ptr1))
			ptr1++;
		++counter;
		ptr0 = (++ptr1);
	}
	
	ptr0 = ptr1 = dst;
	
	np->data = calloc(sizeof(struct token), counter);
	np->num = counter;
	
	int linen = 0;
	int index = 0;
	int flag_inc = 0;
	while (*ptr1) {
		memset(buf, 0, sizeof(buf));
		flag_inc = 0;
		while (*ptr1 && !isspace(*ptr1))
			ptr1++;
		char *ptr2 = ptr1;
		while (*ptr2) {
			if (*ptr2 == '\n' || *ptr2 == '\r') {
				flag_inc++;
			}
			if (!isspace(*ptr2))
				break;
			ptr2++;
		}
		if ((int)(ptr1 - ptr0) <= 0) {
			++ptr1;
			continue;
		}
		strncpy(buf, ptr0, (int)(ptr1 - ptr0));
		ptr0 = (++ptr1);
		strcpy(np->data[index].type, buf);
		np->data[index].line = linen;
		++index;
		np->data[index - 1].inst = get_instances(np, index, buf);
		linen += flag_inc;
		ptr0 = ptr1 = ptr2;
	}

	np->data[index].type[0] = 0;
	index++;
	np->num++;
	free(dst);

	return np;
}

struct token scanner (struct parser *np)
{
	struct token out;
	memset(&out, 0, sizeof(out));
	int index = np->pos++;
	if (np->pos > np->num)
		return out;
	if (!np->data[index].type[0])
		return out;
		
	strcpy(out.type, np->data[index].type);
	out.line = np->data[index].line;
	out.inst = np->data[index].inst;
	
	return out;
}

struct token peek (struct parser *np)
{
	struct token out;
	memset(&out, 0, sizeof(out));
	int index = np->pos;
	if (np->pos > np->num)
		return out;
	if (!np->data[index].type[0])
		return out;
		
	strcpy(out.type, np->data[index].type);
	out.line = np->data[index].line;
	out.inst = np->data[index].inst;
	
	return out;
}

void delete_parser (struct parser *np)
{
	free(np->data);
	free(np);
}
