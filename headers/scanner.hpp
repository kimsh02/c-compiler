#pragma once

#include <memory>
#include <string>

#include "token.hpp"

class Scanner {
    private:
	// inline static const std::unordered_map<std::string, std::string>
	// 	tokens = {
	// 		{	  "(",   "LPAREN" },
	// 		  {	    ")",	 "RPAREN" },
	// 		{	  "{",   "LBRACE" },
	// 		  {	    "}",	 "RBRACE" },
	// 		{	  ",",    "COMMA" },
	// 		  {	    ";",	 "SEMI" },
	// 		{	  "int",	 "kwINT" },
	// 		  {	    "if",	  "kwIF" },
	// 		{	  "else",	  "kwELSE" },
	// 		  {  "while",  "kwWHILE" },
	// 		{ "return", "kwRETURN" },
	// 		  {	    "=",	 "opASSG" },
	// 		{	  "+",    "opADD" },
	// 		  {	    "-",	 "opSUB" },
	// 		{	  "*",    "opMUL" },
	// 		  {	    "/",	 "opDIV" },
	// 		{	  "==",	"opEQ" },
	// 		  {	    "!=",	  "opNE" },
	// 		{	  ">",     "opGT" },
	// 		  {	    ">=",	  "opGE" },
	// 		{	  "<",     "opLT" },
	// 		  {	    "<=",	  "opLE" },
	// 		{	  "&&",	"opAND" },
	// 		  {	    "||",	  "opOR" },
	// 		{	  "!",    "opNOT" }
	// };

	static constexpr int  BUFFER_SIZE     = 4096;
	static constexpr int  NUM_BUFFERS     = 2;
	static constexpr int  MAX_LEXEME_SIZE = 1024;
	static constexpr int  ASCII_SIZE      = 256;
	static constexpr int  mod	      = 256;
	static constexpr char hashchar	      = '\n';
	static constexpr int  bitShift	      = 1;

	// static constexpr std::string ht[ASCII_SIZE] = {
	// 	[(('(' << bitShift) ^ hashchar) % mod] = "LPAREN",
	// 	[((')' << bitShift) ^ hashchar) % mod] = "RPAREN",
	// 	[(('{' << bitShift) ^ hashchar) % mod] = "LBRACE",
	// 	[(('}' << bitShift) ^ hashchar) % mod] = "RBRACE",
	// 	[((',' << bitShift) ^ hashchar) % mod] = "COMMA",
	// 	[((';' << bitShift) ^ hashchar) % mod] = "SEMI",
	// 	[(('=' << bitShift) ^ hashchar) % mod] = "opASSG",
	// 	[(('+' << bitShift) ^ hashchar) % mod] = "opADD",
	// 	[(('-' << bitShift) ^ hashchar) % mod] = "opSUB",
	// 	[(('*' << bitShift) ^ hashchar) % mod] = "opMUL",
	// 	[(('/' << bitShift) ^ hashchar) % mod] = "opDIV",
	// 	[(('>' << bitShift) ^ hashchar) % mod] = "opGT",
	// 	[(('<' << bitShift) ^ hashchar) % mod] = "opLT",
	// 	[(('!' << bitShift) ^ hashchar) % mod] = "opNOT",
	// 	[(('>' << bitShift) ^ '=') % mod]      = "opGE",
	// 	[(('i' << bitShift) ^ 'n') % mod]      = "kwINT",
	// 	[(('i' << bitShift) ^ 'f') % mod]      = "kwIF",
	// 	[(('e' << bitShift) ^ 'l') % mod]      = "kwELSE",
	// 	[(('w' << bitShift) ^ 'h') % mod]      = "kwWHILE",
	// 	[(('r' << bitShift) ^ 'e') % mod]      = "kwRETURN",
	// 	[(('=' << bitShift) ^ '=') % mod]      = "opEQ",
	// 	[(('!' << bitShift) ^ '=') % mod]      = "opNE",
	// 	[(('<' << bitShift) ^ '=') % mod]      = "opLE",
	// 	[(('&' << bitShift) ^ '&') % mod]      = "opAND",
	// 	[(('|' << bitShift) ^ '|') % mod]      = "opOR",
	// };

	// static constexpr int NUM_TOKENS = 25;

	// static constexpr std::string lexemes[NUM_TOKENS] = {
	// 	"(",	 ")",	   "{",	 "}",  ",",  ";",  "=",	  "+",	"-",
	// 	"*",	 "/",	   ">",	 "<",  "!",  ">=", "int", "if", "else",
	// 	"while", "return", "==", "!=", "<=", "&&", "||"
	// };

	// static constexpr std::string tokens[NUM_TOKENS] = {
	// 	"LPAREN", "RPAREN", "LBRACE", "RBRACE",	 "COMMA",
	// 	"SEMI",	  "opASSG", "opADD",  "opSUB",	 "opMUL",
	// 	"opDIV",  "opGT",   "opLT",   "opNOT",	 "opGE",
	// 	"kwINT",  "kwIF",   "kwELSE", "kwWHILE", "kwRETURN",
	// 	"opEQ",	  "opNE",   "opLE",   "opAND",	 "opOR"
	// };

	std::string tok[ASCII_SIZE];
	std::string lex[ASCII_SIZE];

	bool singles[ASCII_SIZE];
	bool doubles[ASCII_SIZE];

	// Next time just use one buffer array with the size of two buffers
	char   buffer[NUM_BUFFERS][BUFFER_SIZE];
	int    curr_buffer;
	int    buffer_size[NUM_BUFFERS];
	int    buffer_pos;
	size_t ln;

	void buildTokenLexemeHT();
	void refillBuffer();
	bool getLexemeStart(char *&, bool &);
	bool getLexemeLength(char *, char *&, int (&)[NUM_BUFFERS], bool &);
	void printToken(const std::string &, const std::string &);
	bool isIntCon(const std::string &lexeme);
	bool checkDouble(char, char);
	int  hash(char, char);
	std::string lnToString();

	std::string getNextLexeme();

    public:
	// bool keychars[ASCII_SIZE];
	Scanner();
	bool		       printNextToken();
	std::unique_ptr<Token> getNextToken();
};
