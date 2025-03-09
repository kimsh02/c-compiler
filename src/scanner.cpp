#include "scanner.hpp"

#include <unistd.h>

// #include <algorithm>
#include <charconv>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "token.hpp"

void Scanner::buildTokenLexemeHT()
{
	tok[(('(' << bitShift) ^ hashchar) % mod] = "LPAREN";	/* 90 */
	tok[((')' << bitShift) ^ hashchar) % mod] = "RPAREN";	/* 88 */
	tok[(('{' << bitShift) ^ hashchar) % mod] = "LBRACE";	/* 252 */
	tok[(('}' << bitShift) ^ hashchar) % mod] = "RBRACE";	/* 240 */
	tok[((',' << bitShift) ^ hashchar) % mod] = "COMMA";	/* 82 */
	tok[((';' << bitShift) ^ hashchar) % mod] = "SEMI";	/* 124 */
	tok[(('=' << bitShift) ^ hashchar) % mod] = "opASSG";	/* 112 */
	tok[(('+' << bitShift) ^ hashchar) % mod] = "opADD";	/* 92 */
	tok[(('-' << bitShift) ^ hashchar) % mod] = "opSUB";	/* 80 */
	tok[(('*' << bitShift) ^ hashchar) % mod] = "opMUL";	/* 94 */
	tok[(('/' << bitShift) ^ hashchar) % mod] = "opDIV";	/* 84 */
	tok[(('>' << bitShift) ^ hashchar) % mod] = "opGT";	/* 118 */
	tok[(('<' << bitShift) ^ hashchar) % mod] = "opLT";	/* 114 */
	tok[(('!' << bitShift) ^ hashchar) % mod] = "opNOT";	/* 72  */
	tok[(('>' << bitShift) ^ '=') % mod]	  = "opGE";	/* 65 */
	tok[(('i' << bitShift) ^ 'n') % mod]	  = "kwINT";	/* 188 */
	tok[(('i' << bitShift) ^ 'f') % mod]	  = "kwIF";	/* 180 */
	tok[(('e' << bitShift) ^ 'l') % mod]	  = "kwELSE";	/* 166 */
	tok[(('w' << bitShift) ^ 'h') % mod]	  = "kwWHILE";	/* 134 */
	tok[(('r' << bitShift) ^ 'e') % mod]	  = "kwRETURN"; /* 129 */
	tok[(('=' << bitShift) ^ '=') % mod]	  = "opEQ";	/* 71 */
	tok[(('!' << bitShift) ^ '=') % mod]	  = "opNE";	/* 127 */
	tok[(('<' << bitShift) ^ '=') % mod]	  = "opLE";	/* 69 */
	tok[(('&' << bitShift) ^ '&') % mod]	  = "opAND";	/* 106 */
	tok[(('|' << bitShift) ^ '|') % mod]	  = "opOR";	/* 132 */

	lex[(('(' << bitShift) ^ hashchar) % mod] = "(";
	lex[((')' << bitShift) ^ hashchar) % mod] = ")";
	lex[(('{' << bitShift) ^ hashchar) % mod] = "{";
	lex[(('}' << bitShift) ^ hashchar) % mod] = "}";
	lex[((',' << bitShift) ^ hashchar) % mod] = ",";
	lex[((';' << bitShift) ^ hashchar) % mod] = ";";
	lex[(('=' << bitShift) ^ hashchar) % mod] = "=";
	lex[(('+' << bitShift) ^ hashchar) % mod] = "+";
	lex[(('-' << bitShift) ^ hashchar) % mod] = "-";
	lex[(('*' << bitShift) ^ hashchar) % mod] = "*";
	lex[(('/' << bitShift) ^ hashchar) % mod] = "/";
	lex[(('>' << bitShift) ^ hashchar) % mod] = ">";
	lex[(('<' << bitShift) ^ hashchar) % mod] = "<";
	lex[(('!' << bitShift) ^ hashchar) % mod] = "!";
	lex[(('>' << bitShift) ^ '=') % mod]	  = ">=";
	lex[(('i' << bitShift) ^ 'n') % mod]	  = "int";
	lex[(('i' << bitShift) ^ 'f') % mod]	  = "if";
	lex[(('e' << bitShift) ^ 'l') % mod]	  = "else";
	lex[(('w' << bitShift) ^ 'h') % mod]	  = "while";
	lex[(('r' << bitShift) ^ 'e') % mod]	  = "return";
	lex[(('=' << bitShift) ^ '=') % mod]	  = "==";
	lex[(('!' << bitShift) ^ '=') % mod]	  = "!=";
	lex[(('<' << bitShift) ^ '=') % mod]	  = "<=";
	lex[(('&' << bitShift) ^ '&') % mod]	  = "&&";
	lex[(('|' << bitShift) ^ '|') % mod]	  = "||";
}

Scanner::Scanner()
	: curr_buffer{ 1 }
	, buffer_size{ 0 }
	, buffer_pos{ 0 }
	, ln{ 1 }
{
	refillBuffer();
	for (int i = 0; i < ASCII_SIZE; i++) {
		// if (i == 33 || i == 32 || i == 10 || i == 38 ||
		//     (i > 39 && i < 46) || i == 47 || (i > 58 && i < 63) ||
		//     (i > 122 && i < 126) || (i > 46 && i < 58) ||
		//     (i > 64 && i < 91) || (i > 96 && i < 123)) {
		// 	keychars[i] = true;
		// } else {
		// 	keychars[i] = false;
		// }
		if ((i > 39 && i < 46) || i == 47 || i == 59 || i == 123 ||
		    i == 125) {
			singles[i] = true;
		} else {
			singles[i] = false;
		}
		if (i == 33 || i == 38 || (i > 59 && i < 63) || i == 124) {
			doubles[i] = true;
		} else {
			doubles[i] = false;
		}
	}
	buildTokenLexemeHT();
}

void Scanner::refillBuffer()
{
	curr_buffer = !curr_buffer;
	buffer_size[curr_buffer] =
		read(STDIN_FILENO, buffer[curr_buffer], BUFFER_SIZE);
	buffer_pos = 0;
}

bool Scanner::getLexemeStart(char *&start1, bool &slash)
{
	bool slash_last = false;
	bool comment	= false;
	bool star_last	= false;
	bool cmt_ended	= false;
	// Find start of lexeme

	while (true) {
		// Refill buffer if necessary
		if (buffer_pos >= buffer_size[curr_buffer]) {
			// Source code is less than BUFFER_SIZE and no valid tokens
			// if (buffer_size[curr_buffer] < 4096) {
			// 	// But if slash was last, then it is valid token
			// 	// (but not really)
			// 	if (slash_last) {
			// 		start1 = &buffer[curr_buffer]
			// 				[--buffer_pos];
			// 		break;
			// 	} else {
			// 		return false;
			// 	}
			// }
			refillBuffer();
			// Source code is exactly BUFFER_SIZE and no valid tokens
			if (buffer_size[curr_buffer] <= 0) {
				// Again if slash was last, then it is valid token
				// (but not really)

				if (slash_last) {
					if (cmt_ended) {
						return false;
					}
					curr_buffer = !curr_buffer;
					start1 =
						&buffer[curr_buffer]
						       [buffer_size[curr_buffer] -
							1];
					break;
				} else {
					return false;
				}
			}
		}
		// Count new lines
		if (buffer[curr_buffer][buffer_pos] == 10) {
			ln += 1;
		}
		// Skip whitespace
		if (buffer[curr_buffer][buffer_pos] < 33 && !comment) {
			if (slash_last) {
				if (buffer_pos == 0) {
					slash	    = true;
					curr_buffer = !curr_buffer;
					buffer_pos  = BUFFER_SIZE - 1;
					start1 =
						&buffer[curr_buffer][buffer_pos];
				} else {
					start1 = &buffer[curr_buffer]
							[--buffer_pos];
				}
				break;
			}
			// cmt_ended = false;
			buffer_pos++;
			continue;
		}
		// Skip comments but not divide or star sign
		// If slash found
		// std::cout << buffer[curr_buffer][buffer_pos] << "\n";
		// std::cout << buffer_pos << "\n";
		// std::cout << buffer_size[curr_buffer] << "\n";
		// std::cout << buffer[curr_buffer][buffer_pos + 1] << "\n";
		// std::cout << buffer[curr_buffer][buffer_pos + 2] << "\n";
		if (buffer[curr_buffer][buffer_pos] == 47) {
			if (star_last) {
				star_last = false;
				comment	  = false;
				// slash_last = true;
				cmt_ended = true;
				buffer_pos++;
				continue;
			} else if (slash_last) {
				if (comment) {
					buffer_pos++;
					continue;
				}
				if (buffer_pos == 0) {
					slash	    = true;
					curr_buffer = !curr_buffer;
					buffer_pos  = BUFFER_SIZE - 1;
					start1 =
						&buffer[curr_buffer][buffer_pos];
				} else {
					start1 = &buffer[curr_buffer]
							[--buffer_pos];
				}
				// start1 = &buffer[curr_buffer][--buffer_pos];
				break;
			} else {
				// cmt_ended  = false;
				slash_last = true;
				buffer_pos++;
				continue;
			}
		}

		// If star found
		if (buffer[curr_buffer][buffer_pos] == 42) {
			if (slash_last && !comment) {
				slash_last = false;
				comment	   = true;
				buffer_pos++;
				continue;
			}
			if (comment) {
				star_last = true;
				buffer_pos++;
				continue;
			}
		}
		// If we're in a comment
		if (comment) {
			// std::cout << "In comment.\n";
			star_last  = false;
			slash_last = false;
			cmt_ended  = false;
			buffer_pos++;
			continue;
		}
		// Set starting position of lexeme and break
		if (slash_last) {
			if (cmt_ended) {
				start1 = &buffer[curr_buffer][buffer_pos];
			} else {
				if (buffer_pos == 0) {
					slash	    = true;
					curr_buffer = !curr_buffer;
					buffer_pos  = BUFFER_SIZE - 1;
					start1 =
						&buffer[curr_buffer][buffer_pos];
				} else {
					start1 = &buffer[curr_buffer]
							[--buffer_pos];
				}
			}
		}
		start1 = &buffer[curr_buffer][buffer_pos];
		break;
	}
	return true;
}

bool Scanner::checkDouble(char s, char c)
{
	if (s == '=' || s == '!' || s == '>' || s == '<') {
		if (c != '=') {
			return false;
		}
	} else if (s == '&') {
		if (c != '&') {
			return false;
		}
	} else if (s == '|') {
		if (c != '|') {
			return false;
		}
	}
	return true;
}

bool Scanner::getLexemeLength(char *start1, char	   *&start2,
			      int (&len)[NUM_BUFFERS], bool &slash)
{
	bool split	 = false;
	bool double_last = false;

	split = *start1;
	split = 0;

	// Find length of lexeme
	while (true) {
		// Refill buffer if necessary
		if (buffer_pos >= buffer_size[curr_buffer]) {
			// File ends
			// if (buffer_size[curr_buffer] < 4096) {
			// 	break;
			// }
			// If next buffer was already filled from slash
			// if (slash) {
			// 	curr_buffer = !curr_buffer;
			// 	buffer_pos  = 0;
			// 	continue;
			// }
			refillBuffer();
			// File ended after exactly BUFFER_SIZE bytes
			if (buffer_size[curr_buffer] <= 0) {
				break;
			}
			// If buffer has to be refilled, then lexeme is split
			// Across buffers
			split  = true;
			start2 = &buffer[curr_buffer][buffer_pos];
		}
		// Check size of lexeme
		if (len[curr_buffer] + len[!curr_buffer] > MAX_LEXEME_SIZE) {
			throw std::runtime_error(
				"Exceded max token size! Please use shorter identifiers!\n");
		}
		// Length of lexeme determined on first whitespace
		if (buffer[curr_buffer][buffer_pos] < 33) {
			break;
		}
		// Or singles
		if (singles[(int)buffer[curr_buffer][buffer_pos]]) {
			// If token is terminated by single
			if (len[!split] + len[split] > 0) {
				break;
			}
			// Else single is token
			if (slash) {
				buffer_pos  = 0;
				curr_buffer = !curr_buffer;
			} else {
				buffer_pos++;
			}
			len[split]++;
			// std::cout << "Last slash.\n";
			break;
		}
		// Or doubles
		if (doubles[(int)buffer[curr_buffer][buffer_pos]]) {
			// Token ended by start of double
			if (len[!split] + len[split] > 0 && !double_last) {
				break;
			} else if (double_last) {
				char c = buffer[curr_buffer][buffer_pos];
				if (!checkDouble(*start1, c)) {
					return false;
				} else {
					len[split]++;
					buffer_pos++;
				}
				break;
			}
			double_last = true;
			len[split]++;
			buffer_pos++;
			continue;
		}
		if (double_last) {
			char c = buffer[curr_buffer][buffer_pos];
			if (!checkDouble(*start1, c)) {
				return false;
			} else {
				len[split]++;
				buffer_pos++;
			}
			break;
		}
		len[split]++;
		buffer_pos++;
	}
	return split;
}

std::string Scanner::getNextLexeme()
{
	char *start1	       = nullptr;
	char *start2	       = nullptr;
	int   len[NUM_BUFFERS] = { 0, 0 };
	bool  slash	       = false;

	// If no valid token found, return empty string
	if (!getLexemeStart(start1, slash)) {
		return "";
	}

	// Get length of lexeme, and whether lexeme is split betweeen the two
	// buffers
	bool split = getLexemeLength(start1, start2, len, slash);

	// Copy and return lexeme
	std::string lexeme;
	if (!split) {
		lexeme.reserve(len[split]);
		lexeme.assign(start1, len[split]);
	} else {
		lexeme.reserve(len[!split] + len[split]);
		lexeme.append(start1, len[!split]);
		lexeme.append(start2, len[split]);
	}
	return lexeme;
}

void Scanner::printToken(const std::string &tok, const std::string &lexeme)
{
	std::cout << tok << " : " << lexeme << "\n";
}

bool Scanner::isIntCon(const std::string &lexeme)
{
	int tmp;
	auto [ptr, ec] = std::from_chars(
		lexeme.data(), lexeme.data() + lexeme.size(), tmp);
	return ec == std::errc() && ptr == lexeme.data() + lexeme.size();
}

int Scanner::hash(char first, char second = hashchar)
{
	return ((first << bitShift) ^ second) % mod;
}

std::string Scanner::lnToString()
{
	char tmp_buf[32];
	snprintf(tmp_buf, 32, "%zu", ln);
	return std::string(tmp_buf);
}

std::unique_ptr<Token> Scanner::getNextToken()
{
	auto t = std::make_unique<Token>();

	std::string lexeme{ getNextLexeme() };
	std::string ln{ lnToString() };

	t->ln	  = ln; /* Set line number */
	t->token  = -1; /* Set token to -1 in the beginning */
	t->lexeme = lexeme;

	if (t->lexeme.size() == 0) {
		return t;
	}
	// t->lexeme = lexeme; /* Set lexeme */
	// Find token
	// if (lexeme.size() == 1) { // if keyword with one char
	// 	int h = hash(lexeme.at(0));
	// 	if (std::string tmp = tok[h]; tmp.size() != 0) {
	// 		if (lex[h] == t->lexeme) {
	// 			t->token = h;
	// 			return t;
	// 		}
	// 	}
	// }
	// if (lexeme.size() > 1) { // If keyword
	int h;
	if (t->lexeme.size() == 1) {
		h = hash(lexeme.at(0));
	} else {
		h = hash(lexeme.at(0), lexeme.at(1));
	}
	if (std::string tmp = lex[h]; tmp.size() != 0) {
		if (tmp == lexeme) {
			t->token = h;
			return t;
		}
	}
	// }
	// check INTCON, ID, UNDEF
	char first = lexeme.at(0);
	if (isIntCon(lexeme)) {
		t->token = 256;
	} else if ((first > 64 && first < 91) || (first > 96 && first < 123)) {
		t->token = 257;
	} else {
		t->token = 258;
	}
	return t;
}

bool Scanner::printNextToken()
{
	std::string lexeme = getNextLexeme();
	if (lexeme.size() == 0) {
		return false;
	}
	// Find token
	if (lexeme.size() == 1) { // if keyword with one char
		if (std::string tmp = tok[hash(lexeme.at(0))];
		    tmp.size() != 0) {
			// if (tmp == lexeme) {
			printToken(tmp, lexeme);
			return true;
			// }
		}
	}
	if (lexeme.size() > 1) { // If keyword with two or more chars
		if (std::string tmp = lex[hash(lexeme.at(0), lexeme.at(1))];
		    tmp.size() != 0) {
			if (tmp == lexeme) {
				printToken(
					tok[hash(lexeme.at(0), lexeme.at(1))],
					lexeme);
				return true;
			}
		}
	}
	// check INTCON, ID, UNDEF
	char first = lexeme.at(0);
	if (isIntCon(lexeme)) {
		printToken("INTCON", lexeme);
	} else if ((first > 64 && first < 91) || (first > 96 && first < 123)) {
		printToken("ID", lexeme);
	} else {
		printToken("UNDEF", lexeme);
	}
	return true;
}
