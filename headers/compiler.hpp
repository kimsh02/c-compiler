#pragma once

class Compiler {
    private:
	const int	   argc;
	const char *const *argv;

	int decl_flag;
	int ast_flag;
	int code_flag;

	void parseArgs(void);

    public:
	Compiler(int, const char *const[]);
	void run(void);
};
