#include "symboltablestack.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>

#include "symbol.h"
#include "token.hpp"

// SymbolTableStack::SymbolTableStack(void)
// 	: symbolTableStack{}
// {
// }

void SymbolTableStack::push(void)
{
	// std::cout << "Stack pushed.\n";
	symbolTableStack.emplace_front();
}

void SymbolTableStack::pop(void)
{
	// std::cout << "Stack popped.\n";
	symbolTableStack.pop_front();
}

void SymbolTableStack::setFuncNumParams(const std::string &funcID,
					std::size_t	   numParams)
{
	for (auto &symbolTable : symbolTableStack) {
		if (symbolTable.contains(funcID)) {
			symbolTable.find(funcID)->second.numParams = numParams;
			return;
		}
	}
	std::cout
		<< "No such function found to add params to. Should not happen.\n";
}

void SymbolTableStack::insert(const Token &token, SymbolType st)
{
	// std::cout << "Inserting " << token.lexeme << "\n";

	bool inserted =
		symbolTableStack.front()
			.emplace(std::move(token.lexeme), Symbol{ st, 0 })
			.second;
	if (!inserted) {
		std::cerr << "ERROR on LINE " << token.ln << " at lexeme \""
			  << token.lexeme << "\"!\n";
		exit(1);
	}
}

void SymbolTableStack::find(const Token &token, SymbolType st) const
{
	for (const auto &symbolTable : symbolTableStack) {
		auto it = symbolTable.find(token.lexeme);
		if (it != symbolTable.end()) {
			const auto &symbol = it->second;
			if (symbol.symbolType == st) {
				return;
			} else {
				std::cerr << "ERROR on LINE " << token.ln
					  << " at lexeme \"" << token.lexeme
					  << "\"!\n";
				exit(1);
			}
		}
	}
	std::cerr << "ERROR on LINE " << token.ln << " at lexeme \""
		  << token.lexeme << "\"!\n";
	exit(1);
}

void SymbolTableStack::checkFuncNumParams(const Token &func,
					  std::size_t  numParams) const
{
	// std::cout << "HERE\n";
	// std::cout << func.lexeme << "\n";
	for (const auto &symbolTable : symbolTableStack) {
		if (symbolTable.contains(func.lexeme)) {
			const auto &symbol =
				symbolTable.find(func.lexeme)->second;
			if (symbol.numParams == numParams) {
				return;
			} else {
				std::cerr << "ERROR on LINE " << func.ln
					  << " at lexeme \"" << func.lexeme
					  << "\"!\n";
				exit(1);
			}
		}
	}
	std::cout
		<< "No such function found to check num params. Should not happen.\n";
	// std::cerr << "ERROR on LINE " << func.ln << " at lexeme \""
	// 	  << func.lexeme << "\"!\n";
	// exit(1);
}
