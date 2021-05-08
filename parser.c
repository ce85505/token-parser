#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "testTree.h"

struct token tk;

struct node * program (struct parser *p);
struct node * block (struct parser *p);
struct node * vars (struct parser *p);
struct node * expr (struct parser *p);
struct node * N (struct parser *p);
struct node * A (struct parser *p);
struct node * M (struct parser *p);
struct node * R (struct parser *p);
struct node * mStat (struct parser *p);
struct node * stats (struct parser *p);
struct node * stat (struct parser *p);
struct node * in (struct parser *p);
struct node * out (struct parser *p);
struct node * iffy (struct parser *p);
struct node * loop (struct parser *p);
struct node * assign (struct parser *p);
struct node * r0 (struct parser *p);
struct node * cond (struct parser *np);
int errf;

int is_identifier (const char *type)
{
	if (!strcmp(type, "out") || !isalnum(type[0]) || !strcmp(type, "end"))
		return 0;

	return 1;
}

/* <program>  ->     <vars> <block> */
struct node* program (struct parser *np)
{
	struct node *p = add_node("program");
	p->token = tk;
	p->left = vars(np);
	p->right = block(np);
	return p;
}

/* <block>       ->      begin <vars> <stats> end */
struct node* block (struct parser *np)
{
	if (strcmp(tk.type, "begin"))
		return NULL;
	
	struct node *p = add_node("block");
	p->token = tk; 
	tk=scanner(np);
	if ((p->left = vars(np)) == NULL) {
		;
	}

	if ((p->right = stats(np)) == NULL) {
		printf("%s: error: expected <stats> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}

	if (!strcmp(tk.type, "end")) {
		tk=scanner(np); 
		return p;
	}
	
	printf("%s: error: expected keyword 'end' type %s line %d\n", __func__, tk.type, tk.line);
	delete_tree(p);
	return NULL;
}


/* <stat>           ->      <in> .  | <out> .  | <block> | <if> .  | <loop> .  | <assign> .  */
struct node* stat (struct parser *np)
{
	struct node *tmp;
	if ((tmp = in(np)) != NULL) {
		struct node *p = add_node("stat");
		p->left = tmp;

		if (!strcmp(tk.type, ".")) {
			tk=scanner(np); 
		}
		else {
			printf("%s: error: expected '.' type %s line %d\n", __func__, tk.type, tk.line);
			errf = 1;
			delete_tree(p);
			return NULL;
		}
		return p;
	} else if ((tmp = out(np)) != NULL) {
		struct node *p = add_node("stat");
		p->left = tmp;

		if (!strcmp(tk.type, ".")) {
			tk=scanner(np); 
		}
		else {
			printf("%s: error: expected '.' type %s line %d\n", __func__, tk.type, tk.line);
			errf = 1;
			delete_tree(p);
			return NULL;
		}
		return p;
	} else if ((tmp = block(np)) != NULL) {
		struct node *p = add_node("stat");
		p->left = tmp;
		return p;
	} else if ((tmp = iffy(np)) != NULL) {
		struct node *p = add_node("stat");
		p->left = tmp;

		if (!strcmp(tk.type, ".")) {
			tk=scanner(np); 
		}
		else {
			printf("%s: error: expected '.' type %s line %d\n", __func__, tk.type, tk.line);
			errf = 1;
			delete_tree(p);
			return NULL;
		}
		return p;
	} else if ((tmp = loop(np)) != NULL) {
		struct node *p = add_node("stat");
		p->left = tmp;
		
		if (!strcmp(tk.type, ".")) {
			tk=scanner(np); 
		}
		else {
			printf("%s: error: expected '.' type %s line %d\n", __func__, tk.type, tk.line);
			errf = 1;
			delete_tree(p);
			return NULL;
		}
		return p;
	} else if ((tmp = assign(np)) != NULL) {
		struct node *p = add_node("stat");
		p->left = tmp;

		//tk=scanner(np);
		if (!strcmp(tk.type, ".")) {
			tk=scanner(np); 
		}
		else {
			printf("%s: error: expected '.' type %s line %d\n", __func__, tk.type, tk.line);
			errf = 1;
			delete_tree(p);
			return NULL;
		}
		return p;
	} else {
		return NULL;
	}
}

/* <expr>        ->      <N> - <expr>  | <N> 
*/
struct node* expr (struct parser *np)
{
	struct node *p = add_node("expr");
	if ((p->left = N(np)) == NULL) {
		printf("%s: error: expected <N> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}

	if (strcmp(tk.type, "-"))
		return p;

	p->token = tk;
	tk=scanner(np); 
	if ((p->right = expr(np)) != NULL) {
		return p;
	} else {
		printf("%s: error: expected <expr> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
}

/* <N>             ->       <A> / <N> | <A> * <N> | <A> 
*/
struct node* N (struct parser *np)
{
	struct node *p = add_node("N");
	if ((p->left = A(np)) == NULL) {
		printf("%s: error: expected <A> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}

	if (!strcmp(tk.type, "/")) {
		p->token = tk;
		tk=scanner(np);
		if ((p->right = N(np)) != NULL) {
			return p;
		} else {
			printf("%s: error: expected <N> type %s line %d\n", __func__, tk.type, tk.line);
			delete_tree(p);
			return NULL;
		}
	} else if (!strcmp(tk.type, "*")) {
		p->token = tk;
		tk=scanner(np);
		if ((p->right = N(np)) != NULL) {
			return p;
		} else {
			printf("%s: error: expected <N> type %s line %d\n", __func__, tk.type, tk.line);
			delete_tree(p);
			return NULL;
		}
	}
	
	return p;
}

/* <A>             ->        <M> + <A> | <M> 
*/
struct node* A (struct parser *np)
{
	struct node *p = add_node("A");
	if ((p->left = M(np)) == NULL) {
		printf("%s: error: expected <M> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	tk=scanner(np);
	
	if (strcmp(tk.type, "+")) // success
		return p;
	p->token = tk;
	tk=scanner(np);
	if ((p->right = A(np)) != NULL) // success
		return p;

	printf("%s: error: expected <A> type %s line %d\n", __func__, tk.type, tk.line);
	delete_tree(p);
	return NULL;
}

/* <M>              ->     * <M> |  <R> */
struct node* M (struct parser *np)
{
	struct node *p = add_node("M");
	if (!strcmp(tk.type, "*")) {
		p->token = tk;
		tk=scanner(np);
		if ((p->left = M(np)) == NULL) {
			printf("%s: error: expected <M> type %s line %d\n", __func__, tk.type, tk.line);
			delete_tree(p);
			return NULL;
		}
		return p;
	}

	if ((p->left = R(np)) != NULL) // success
		return p;
	
	printf("%s: error: expected <R> type %s line %d\n", __func__, tk.type, tk.line);
	delete_tree(p);
	return NULL;
}

/* <stats>         ->      <stat>  <mStat> */
struct node* stats (struct parser *np)
{
	struct node *p = add_node("stats");

	if ((p->left = stat(np)) == NULL) {
		printf("%s: error: expected <stat> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}

	if ((p->right = mStat(np)) == NULL) {
		;
	}
	return p;
}

/* <out>            ->      out <expr> */
struct node* out (struct parser *np)
{
	if (strcmp(tk.type, "out")) 
		return NULL;
		
	struct node *p = add_node("out");
	p->token = tk; 
	tk=scanner(np);
	if ((p->left = expr(np))) // success
		return p;
	
	printf("%s: error: expected <expr> type %s line %d\n", __func__, tk.type, tk.line);
	delete_tree(p);
	return NULL;
}

/* <mStat>       ->      empty |  <stat>  <mStat> */
struct node* mStat (struct parser *np)
{
	struct node *p = add_node("mStat");
	p->token = tk;

	if ((p->left = stat(np)) == NULL) // success
		return p;

	if ((p->right = mStat(np)) == NULL) {
		;
	}
	
	return p;
}

/* <assign>       ->      Identifier  = <expr>  */
struct node* assign (struct parser *np)
{
	struct token tpt = peek(np);
	if (strcmp(tpt.type, "="))
		return NULL;

	if (!is_identifier(tk.type))
		return NULL;
	struct node* p = add_node("assign");
	p->token = tk;
		
	tk=scanner(np);

	if (strcmp(tk.type, "=")) {
		printf("%s: error: expected '=' type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	tk=scanner(np);
	if ((p->left = expr(np)) != NULL) // success
		return p;
	
	printf("%s: error: expected <expr> type %s line %d\n", __func__, tk.type, tk.line);
	delete_tree(p);
	return NULL;
}

/* <in>              ->      in  Identifier  */
struct node* in (struct parser *np)
{
	if (strcmp(tk.type, "in"))
		return NULL;
		
	struct node *p = add_node("in");
	tk=scanner(np);
	p->token = tk; 
	if (is_identifier(tk.type)) {
		tk=scanner(np);
		return p;
	}
	return NULL;
}

/* <R>              ->      ( <expr> ) | Identifier | Integer */
struct node* R (struct parser *np)
{
	if (!strcmp(tk.type, "(")) {
		struct node *p = add_node("R");	
		tk=scanner(np); 
		if ((p->left = expr(np)) == NULL) {
			printf("%s: error: expected <expr> type %s line %d\n", __func__, tk.type, tk.line);
			delete_tree(p);
			return NULL;
		}

		if (strcmp(tk.type, ")")) {
			printf("%s: error: expected ')' type %s line %d\n", __func__, tk.type, tk.line);
			delete_tree(p);
			return NULL;
		}
		return p;
	} else {
		if (!is_identifier(tk.type)) {
			printf("%s: error: expected identifier type %s line %d\n", __func__, tk.type, tk.line);
			return NULL;
		}
		struct node *p = add_node("R");
		p->token = tk;
		return p;
	}
}

/* <vars>         ->      empty | data Identifier =  Integer  .  <vars>  */
/* <assign>       ->      Identifier  = <expr>  */
/* <vars>         ->      empty | data <assign>  .  <vars>  */

struct node* vars (struct parser *np)
{
	if (strcmp(tk.type, "data")) {
		struct node *p = add_node("vars");
		p->token = tk;
		return p;
	}
	struct node *p = add_node("vars");
	tk=scanner(np); // consume "data"
	p->token = tk;
	if ((p->left = assign(np)) == NULL) {
		tk=scanner(np);
		if (!is_identifier(p->token.type)) {
			printf("%s: error: expected identifier type %s line %d\n", __func__, tk.type, tk.line);
			delete_tree(p);
			return NULL;
		}
	}
	
	if (strcmp(tk.type, ".")) {
		printf("%s: error: expected '.' type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	tk=scanner(np); 
	if ((p->right = vars(np))) {
		;
	}
	return p;
}

/* <if>               ->      iffy [ <cond> ] then <stat> */
struct node* iffy (struct parser *np)
{
	if (strcmp(tk.type, "iffy"))
		return NULL;
		
	struct node *p = add_node("if");
	tk=scanner(np);
	if (strcmp(tk.type, "[")) {
		printf("%s: error: expected '[' type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	tk=scanner(np);
	if ((p->left = cond(np)) == NULL) {
		printf("%s: error: expected <cond> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}

	if (strcmp(tk.type, "]")) {
		printf("%s: error: expected ']' type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}

	tk=scanner(np);
	if (strcmp(tk.type, "then")) {
		printf("%s: error: expected keyword 'then' type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	tk=scanner(np);
	if ((p->right = stat(np)) == NULL) {
		printf("%s: error: expected <stat> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	return p;
}

/* <loop>          ->      loop  [ <cond> ]  <stat> */
struct node* loop (struct parser *np)
{
	if (strcmp(tk.type, "loop"))
		return NULL;
		
	struct node *p = add_node("loop");
	tk=scanner(np); 
	if (strcmp(tk.type, "[")) {
		printf("%s: error: expected '[' type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	tk=scanner(np);
	if ((p->left = cond(np)) == NULL) {
		printf("%s: error: expected <cond> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	if (strcmp(tk.type, "]")) {
		printf("%s: error: expected ']' type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	tk=scanner(np);
	if (strcmp(tk.type, "then")) {
		printf("%s: error: expected keyword 'then' type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}

	tk=scanner(np);
	if ((p->right = stat(np)) == NULL) {
		printf("%s: error: expected <stat> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	
	return p;
}

/* <cond> 		-> 	<expr> <RO> <expr> */
struct node* cond (struct parser *np)
{
	struct node *p = add_node("cond");
	if ((p->left = expr(np)) == NULL) {
		printf("%s: error: expected <expr> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	if ((p->middle = r0(np)) == NULL) {
		printf("%s: error: expected <r0> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	if ((p->right = expr(np)) == NULL) {
		printf("%s: error: expected <expr> type %s line %d\n", __func__, tk.type, tk.line);
		delete_tree(p);
		return NULL;
	}
	return p;
}

/* <RO>            ->      < | <  <  (two tokens >)  | >  | >  > (two tokens) |  == (one token ==) |   <  >    (two tokens) */
struct node* r0 (struct parser *np)
{
	struct node* p = add_node("r0");

	p->token = tk;
	tk=scanner(np);
	
	if (tk.type[0] == '<' || tk.type[0] == '>' || tk.type[0] == '=') {
		p->token1 = tk;
		tk=scanner(np);
	}
	
	return p;
}

struct node* parser (struct parser *np)
{
	struct node* root; 
	tk=scanner(np);
	root = program(np);

	if (tk.type[0] == 0 && !errf)
		return root;

	return NULL;
}
