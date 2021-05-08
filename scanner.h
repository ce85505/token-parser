#ifndef SCANNER_H
#define SCANNER_H

#include "parser.h"

struct parser *init_parser (int file);
int get_next (struct parser *np, struct token *out);
void delete_parser (struct parser *np);
struct token scanner (struct parser *np);
struct token peek (struct parser *np);

#endif