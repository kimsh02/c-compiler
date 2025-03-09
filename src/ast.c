#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void freeAST(ASTNode *head)
{
	if (head) {
		freeAST(*(head->children));
		freeAST(*(head->children + 1));
		freeAST(*(head->children + 2));
		free(head->name);
		free(head);
	}
}

void child1(ASTNode *p, ASTNode *c)
{
	*(p->children) = c;
}

void child2(ASTNode *p, ASTNode *c)
{
	*(p->children + 1) = c;
}

void child3(ASTNode *p, ASTNode *c)
{
	*(p->children + 2) = c;
}

void setNode(ASTNode *node, NodeType ntype, const Symbol *symbol,
	     const char *name, int num, int val)
{
	node->ntype  = ntype;
	node->symbol = symbol;
	if (name) {
		node->name = (char *)malloc(strlen(name) + 1);
		strcpy(node->name, name);
	}
	node->num = num;
	node->val = val;
}

ASTNode *makeASTNode(void)
{
	ASTNode *ret = (ASTNode *)calloc(1, sizeof(ASTNode));
	return ret;
}

NodeType ast_node_type(void *ptr)
{
	return ((ASTNode *)ptr)->ntype;
}

const char *func_def_name(void *ptr)
{
	/* printf("%s\n", ((ASTNode *)ptr)->name); */
	return ((ASTNode *)ptr)->name;
}

int func_def_nargs(void *ptr)
{
	return ((ASTNode *)ptr)->num;
}

const char *func_def_argname(void *ptr, int n)
{
	ASTNode *curr = *((ASTNode *)ptr)->children;
	if (n > 0 && n <= ((ASTNode *)ptr)->num) {
		for (int i = 0; i < n - 1; i++) {
			curr = *(curr->children);
		}
		return curr->name;
	} else {
		return NULL;
	}
}

void *func_def_body(void *ptr)
{
	/* Second child is func body */
	return *(((ASTNode *)ptr)->children + 1);
}

const char *func_call_callee(void *ptr)
{
	return ((ASTNode *)ptr)->name;
}

void *func_call_args(void *ptr)
{
	return *(((ASTNode *)ptr)->children);
}

void *stmt_list_head(void *ptr)
{
	return *(((ASTNode *)ptr)->children);
}

void *stmt_list_rest(void *ptr)
{
	return *(((ASTNode *)ptr)->children + 1);
}

void *expr_list_head(void *ptr)
{
	return *(((ASTNode *)ptr)->children);
}

void *expr_list_rest(void *ptr)
{
	return *(((ASTNode *)ptr)->children + 1);
}

const char *expr_id_name(void *ptr)
{
	return ((ASTNode *)ptr)->name;
}

int expr_intconst_val(void *ptr)
{
	return ((ASTNode *)ptr)->val;
}

void *expr_operand_1(void *ptr)
{
	return *(((ASTNode *)ptr)->children);
}

void *expr_operand_2(void *ptr)
{
	return *(((ASTNode *)ptr)->children + 1);
}

void *stmt_if_expr(void *ptr)
{
	return *(((ASTNode *)ptr)->children);
}

void *stmt_if_then(void *ptr)
{
	return *(((ASTNode *)ptr)->children + 1);
}

void *stmt_if_else(void *ptr)
{
	return *(((ASTNode *)ptr)->children + 2);
}

const char *stmt_assg_lhs(void *ptr)
{
	return (*(((ASTNode *)ptr)->children))->name;
}

void *stmt_assg_rhs(void *ptr)
{
	return *(((ASTNode *)ptr)->children + 1);
}

void *stmt_while_expr(void *ptr)
{
	return *(((ASTNode *)ptr)->children);
}

void *stmt_while_body(void *ptr)
{
	return *(((ASTNode *)ptr)->children + 1);
}

void *stmt_return_expr(void *ptr)
{
	return *(((ASTNode *)ptr)->children);
}
