#pragma once

#include <memory>

#include "ast.h"
#include "scanner.hpp"
#include "symboltablestack.hpp"
#include "token.hpp"

class Parser {
    private:
	enum TokenHash {
		LPAREN	 = 90,
		RPAREN	 = 88,
		LBRACE	 = 252,
		RBRACE	 = 240,
		COMMA	 = 82,
		SEMI	 = 124,
		opASSG	 = 112,
		opADD	 = 92,
		opSUB	 = 80,
		opMUL	 = 94,
		opDIV	 = 84,
		opGT	 = 118,
		opLT	 = 114,
		opNOT	 = 72,
		opGE	 = 65,
		kwINT	 = 188,
		kwIF	 = 180,
		kwELSE	 = 166,
		kwWHILE	 = 134,
		kwRETURN = 129,
		opEQ	 = 71,
		opNE	 = 127,
		opLE	 = 69,
		opAND	 = 106,
		opOR	 = 132,
		INTCON	 = 256,
		ID	 = 257,
		UNDEF	 = 258,
	};

	Scanner s;

	std::unique_ptr<Token> curr;
	std::unique_ptr<Token> id_token;

	std::string funcID;
	std::size_t funcNumParams;
	Token	    func;
	SymbolType  symbolType;

	int intcon;

	SymbolTableStack symbolTableStack;

	const int decl_flag;
	const int ast_flag;
	const int code_flag;

	void	 prog(void);
	void	 type(void);
	void	 func_defn(void);
	ASTNode *opt_formals(void);
	ASTNode *formals(void);
	void	 var_decl_continue(void);
	void	 var_decl(void);
	void	 id_list(void);
	void	 opt_var_decls(void);
	ASTNode *opt_stmt_list(void);
	ASTNode *stmt(void);
	ASTNode *fn_call(void);
	ASTNode *opt_expr_list(void);
	void	 decl_or_func(void);

	ASTNode *if_stmt(void);
	ASTNode *while_stmt(void);
	ASTNode *return_stmt(void);
	ASTNode *assg_stmt(void);
	ASTNode *expr_list(void);
	ASTNode *bool_exp(void);
	ASTNode *arith_exp(void);
	ASTNode *relop(void);
	ASTNode *fn_call_or_assg_stmt(void);

	void match(TokenHash);

	void decl_push(void);
	void decl_pop(void);
	void decl_insert(void);
	void decl_find(void);
	void decl_setFuncNumParams(bool);
	void decl_setFuncToken(void);
	void decl_setFuncID(void);
	void decl_funcNumParamsIncr(void);
	void decl_checkFuncNumParams(void);
	void decl_if(void (Parser::*)(void));
	void setSymbolType(SymbolType);

	void ast_if(ASTNode *);

    public:
	Parser(int, int, int);
	void setFlags(int, int, int);
	void run(void);
};
