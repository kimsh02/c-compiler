#pragma once

#include <string>

struct Token {
	std::string ln;
	int	    token;
	std::string lexeme;

	void printToken() const;
};
