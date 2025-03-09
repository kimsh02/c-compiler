#include "token.hpp"

#include <iostream>

void Token::printToken() const
{
	std::cout << "LN: " << ln << ", " << token << ": " << lexeme << "\n";
}
