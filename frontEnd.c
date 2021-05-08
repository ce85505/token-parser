#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "scanner.h"
#include "testTree.h"
#include "parser.h"

int main (int argc, const char *argv[])
{
  	int fd = STDIN_FILENO;
  	char buf[256];
  	struct parser *root;
  	char ofile[128], *ptr;
  	
  	if (argc > 1) {
  		snprintf(buf, sizeof(buf), "%s.sp2020", argv[1]);
		if ((fd = open(buf, O_RDWR, 0777)) < 0) {
			if ((fd = open(argv[1], O_RDONLY, 0777)) < 0) {
				perror("open");
				return -1;
			} else { //strip extension
				strncpy(ofile, buf, sizeof(ofile));
				ptr = &ofile[0] + strlen(ofile) - 1;
				while (ptr > ofile && *ptr != '.')
					--ptr;
				if (ptr <= ofile) {
					printf("error finding file %s\n", buf);
					close(fd);
					return -1;
				}
				*ptr = 0;
			}
		} else {
			strncpy(ofile, argv[1], sizeof(ofile));
		}
  	} else {
  		strncpy(ofile, "output", sizeof(ofile));
  	}

	root = init_parser(fd);
	if (fd != STDIN_FILENO)
		close(fd);
	if (root == NULL)
		return -1;

	struct node *proot = parser(root);
	if (proot == NULL) {
		printf("error: eof not reached\n");
	} else {
		printPreorder(proot, 0, ofile);
		printf("parsing ok\n");
	}
		
	delete_tree (proot);
	free(proot);
	
	delete_parser(root);
	
	return 0;
}
