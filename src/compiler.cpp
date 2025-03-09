#include "compiler.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "parser.hpp"

Compiler::Compiler(int c, const char *const v[])
	: argc{ c }
	, argv{ v }
	, decl_flag{ 0 }
	, ast_flag{ 0 }
	, code_flag{ 0 }
{
	parseArgs();
}

void Compiler::run(void)
{
	Parser parser{ decl_flag, ast_flag, code_flag };
	parser.run();
}

void Compiler::parseArgs(void)
{
	for (int i = 0; i < argc; i++) {
		if (**(argv + i) == '-') {
			if (strcmp(*(argv + i), "--chk_decl") == 0) {
				decl_flag = 1;
			} else if (strcmp(*(argv + i), "--print_ast") == 0) {
				ast_flag = 1;
			} else if (strcmp(*(argv + i), "--gen_code") == 0) {
				code_flag = 1;
			} else {
				fprintf(stderr,
					"Unrecognized option: %s\n",
					*(argv + i));
				std::exit(EXIT_FAILURE);
			}
		}
	}
}
