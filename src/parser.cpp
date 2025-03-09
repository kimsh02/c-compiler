#include "parser.hpp"

#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

#include "ast.h"
#include "scanner.hpp"

Parser::Parser(int d, int a, int c)
	: decl_flag{ d }
	, ast_flag{ a }
	, code_flag{ c }
{
}

void Parser::match(TokenHash exp)
{
	// curr->printToken();
	if (curr->token != exp) {
		std::cerr << "ERROR on LINE " << curr->ln << " at lexeme \""
			  << curr->lexeme << "\"!\n";
		exit(1);
	} else {
		if (exp == ID) {
			id_token = std::move(curr);
		} else if (exp == INTCON) {
			const auto &intStr{ curr->lexeme };
			std::from_chars(intStr.data(),
					intStr.data() + intStr.size(),
					intcon);
		}
		curr = s.getNextToken();
	}
}

void Parser::type(void)
{
	match(kwINT);
}

ASTNode *Parser::opt_formals(void)
{
	if (curr->token == kwINT) {
		return formals();
	}
	return NULL;
}

void Parser::opt_var_decls(void)
{
	if (curr->token == kwINT) {
		type();
		var_decl();
		opt_var_decls();
	}
}

ASTNode *Parser::opt_stmt_list(void)
{
	ASTNode *stmtList = NULL;

	switch (curr->token) {
	case ID:
	case kwWHILE:
	case kwIF:
	case kwRETURN:
	case LBRACE:
	case SEMI:
		stmtList = makeASTNode();
		setNode(stmtList, STMT_LIST, NULL, NULL, 0, 0);
		child1(stmtList, stmt());
		child2(stmtList, opt_stmt_list());
	}
	return stmtList;
}

ASTNode *Parser::fn_call_or_assg_stmt(void)
{
	if (curr->token == LPAREN) {
		ASTNode *funcCall = fn_call();
		match(SEMI);
		return funcCall;
	} else {
		return assg_stmt();
	}
}

void Parser::decl_setFuncToken(void)
{
	func = *id_token.get();
}

ASTNode *Parser::assg_stmt(void)
{
	setSymbolType(VAR);
	decl_if(&Parser::decl_find);

	match(opASSG);

	ASTNode *assgStmt = makeASTNode();
	setNode(assgStmt, ASSG, NULL, NULL, 0, 0);
	ASTNode *assgLhs = makeASTNode();
	setNode(assgLhs, IDENTIFIER, NULL, id_token->lexeme.c_str(), 0, 0);
	child1(assgStmt, assgLhs);

	child2(assgStmt, arith_exp());

	match(SEMI);

	return assgStmt;
}

ASTNode *Parser::stmt(void)
{
	ASTNode *head = NULL;
	if (curr->token == ID) {
		match(ID);
		head = fn_call_or_assg_stmt();
	} else if (curr->token == kwWHILE) {
		head = while_stmt();
	} else if (curr->token == kwIF) {
		head = if_stmt();
	} else if (curr->token == kwRETURN) {
		head = return_stmt();
	} else if (curr->token == LBRACE) {
		match(LBRACE);
		decl_if(&Parser::decl_push);

		head = opt_stmt_list();
		match(RBRACE);

		decl_if(&Parser::decl_pop);

	} else {
		match(SEMI);
	}
	return head;
}

ASTNode *Parser::if_stmt(void)
{
	match(kwIF);

	ASTNode *ifStmt = makeASTNode();
	setNode(ifStmt, IF, NULL, NULL, 0, 0);

	match(LPAREN);

	child1(ifStmt, bool_exp());

	match(RPAREN);

	child2(ifStmt, stmt());

	if (curr->token == kwELSE) {
		match(kwELSE);
		child3(ifStmt, stmt());
	}

	return ifStmt;
}

ASTNode *Parser::while_stmt(void)
{
	match(kwWHILE);

	ASTNode *whileStmt = makeASTNode();
	setNode(whileStmt, WHILE, NULL, NULL, 0, 0);

	match(LPAREN);

	child1(whileStmt, bool_exp());

	match(RPAREN);
	child2(whileStmt, stmt());

	return whileStmt;
}

ASTNode *Parser::return_stmt(void)
{
	match(kwRETURN);

	ASTNode *returnStmt = makeASTNode();
	setNode(returnStmt, RETURN, NULL, NULL, 0, 0);

	if (curr->token == SEMI) {
		match(SEMI);
	} else {
		child1(returnStmt, arith_exp());
		match(SEMI);
	}
	return returnStmt;
}

ASTNode *Parser::fn_call(void)
{
	setSymbolType(FUNC);
	decl_if(&Parser::decl_find);
	decl_if(&Parser::decl_setFuncToken);

	ASTNode *funcCall = makeASTNode();
	setNode(funcCall, FUNC_CALL, NULL, id_token->lexeme.c_str(), 0, 0);

	match(LPAREN);
	child1(funcCall, opt_expr_list());
	decl_if(&Parser::decl_checkFuncNumParams);

	funcCall->num = funcNumParams;

	match(RPAREN);

	return funcCall;
}

ASTNode *Parser::opt_expr_list(void)
{
	if (curr->token == ID || curr->token == INTCON) {
		return expr_list();
	}
	return NULL;
}

ASTNode *Parser::expr_list(void)
{
	ASTNode *exprList = makeASTNode();
	setNode(exprList, EXPR_LIST, NULL, NULL, 0, 0);
	child1(exprList, arith_exp());

	decl_if(&Parser::decl_funcNumParamsIncr);

	if (curr->token == COMMA) {
		match(COMMA);
		child2(exprList, expr_list());
	}

	return exprList;
}

ASTNode *Parser::bool_exp(void)
{
	ASTNode *exprLhs = arith_exp();
	ASTNode *exprOp	 = relop();
	ASTNode *exprRhs = arith_exp();

	child1(exprOp, exprLhs);
	child2(exprOp, exprRhs);

	return exprOp;
}

ASTNode *Parser::arith_exp(void)
{
	ASTNode *exp = makeASTNode();
	if (curr->token == ID) {
		match(ID);

		setSymbolType(VAR);
		decl_if(&Parser::decl_find);

		setNode(exp, IDENTIFIER, NULL, id_token->lexeme.c_str(), 0, 0);
		return exp;

	} else {
		// std::cout << "HERE\n";
		// std::cout << curr->token << "\n";
		match(INTCON);
		setNode(exp, INTCONST, NULL, NULL, 0, intcon);
		return exp;
	}
}

ASTNode *Parser::relop(void)
{
	ASTNode *relop = makeASTNode();
	NodeType nt;
	switch (curr->token) {
	case opEQ:
		nt = EQ;
		match(opEQ);
		break;
	case opNE:
		nt = NE;
		match(opNE);
		break;
	case opLE:
		nt = LE;
		match(opLE);
		break;
	case opLT:
		nt = LT;
		match(opLT);
		break;
	case opGE:
		nt = GE;
		match(opGE);
		break;
	default:
		nt = GT;
		match(opGT);
	}
	setNode(relop, nt, NULL, NULL, 0, 0);
	return relop;
}

void Parser::setSymbolType(SymbolType st)
{
	symbolType = st;
}

void Parser::ast_if(ASTNode *head)
{
	if (ast_flag) {
		print_ast(head);
	}
}

void Parser::func_defn(void)
{
	// type();
	// match(ID);

	setSymbolType(FUNC);
	decl_if(&Parser::decl_insert);

	/* Always save func name */
	decl_setFuncID();

	match(LPAREN);

	decl_if(&Parser::decl_push);

	ASTNode *funcArgs = opt_formals();

	ASTNode *func = makeASTNode();
	setNode(func, FUNC_DEF, NULL, funcID.c_str(), funcNumParams, 0);

	/* Always save number of func params */
	decl_setFuncNumParams(decl_flag);

	match(RPAREN);
	match(LBRACE);
	opt_var_decls();
	ASTNode *funcBody = opt_stmt_list();
	match(RBRACE);

	child1(func, funcArgs);
	child2(func, funcBody);

	ast_if(func);
	freeAST(func);

	decl_if(&Parser::decl_pop);
}

void Parser::decl_or_func(void)
{
	if (curr->token == LPAREN) {
		func_defn();
	} else {
		var_decl_continue();
	}
}

void Parser::var_decl_continue(void)
{
	setSymbolType(VAR);
	decl_if(&Parser::decl_insert);

	if (curr->token == SEMI) {
		match(SEMI);
	} else {
		match(COMMA);
		var_decl();
	}
}

void Parser::var_decl(void)
{
	id_list();
	match(SEMI);
}

void Parser::id_list(void)
{
	match(ID);

	setSymbolType(VAR);
	decl_if(&Parser::decl_insert);

	if (curr->token == COMMA) {
		match(COMMA);
		id_list();
	}
}

ASTNode *Parser::formals(void)
{
	type();
	match(ID);

	/* Always count number of func params */
	decl_funcNumParamsIncr();
	setSymbolType(VAR);
	decl_if(&Parser::decl_insert);

	ASTNode *arg = makeASTNode();
	setNode(arg, IDENTIFIER, NULL, id_token->lexeme.c_str(), 0, 0);

	if (curr->token == COMMA) {
		match(COMMA);
		child1(arg, formals());
	}

	return arg;
}

void Parser::decl_setFuncNumParams(bool decl_flag)
{
	if (decl_flag) {
		symbolTableStack.setFuncNumParams(funcID, funcNumParams);
	}
	funcNumParams = 0;
}

void Parser::decl_setFuncID(void)
{
	funcID = id_token->lexeme;
}

void Parser::decl_funcNumParamsIncr(void)
{
	funcNumParams++;
}

void Parser::decl_push(void)
{
	symbolTableStack.push();
}

void Parser::decl_pop(void)
{
	symbolTableStack.pop();
}

void Parser::decl_insert(void)
{
	symbolTableStack.insert(*id_token.get(), symbolType);
}

void Parser::decl_checkFuncNumParams(void)
{
	symbolTableStack.checkFuncNumParams(func, funcNumParams);
	funcNumParams = 0;
}

void Parser::decl_find(void)
{
	symbolTableStack.find(*id_token.get(), symbolType);
}

void Parser::decl_if(void (Parser::*df)(void))
{
	if (decl_flag) {
		(this->*df)();
	}
}

void Parser::prog(void)
{
	if (curr->lexeme != "") {
		type();
		match(ID);
		decl_or_func();
		prog();
	}
}

void Parser::run(void)
{
	decl_if(&Parser::decl_push);

	/* println() hack */
	id_token	 = std::make_unique<Token>();
	id_token->lexeme = "println";
	id_token->token	 = ID;

	setSymbolType(FUNC);
	decl_if(&Parser::decl_insert);

	curr = s.getNextToken();
	prog();

	decl_if(&Parser::decl_pop);
}
