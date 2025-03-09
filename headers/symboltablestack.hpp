#pragma once

#include <deque>
#include <string>
#include <unordered_map>

#include "symbol.h"
#include "token.hpp"

class SymbolTableStack {
    private:
	std::deque<std::unordered_map<std::string, Symbol> > symbolTableStack;

    public:
	SymbolTableStack(void) = default;
	void push(void);
	void pop(void);
	void insert(const Token &, SymbolType);
	void find(const Token &, SymbolType) const;
	void setFuncNumParams(const std::string &, std::size_t);
	void checkFuncNumParams(const Token &, std::size_t) const;
};
