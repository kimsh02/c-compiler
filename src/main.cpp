// #include <fstream>
// #include <iostream>
// #include <random>

// #include <iostream>

// #include "ast.h"
#include "compiler.hpp"

int main(int argc, const char *const argv[])
{
	// Scanner scanner;
	// std::random_device rd;	      // Seed generator (uses hardware entropy)
	// std::mt19937	   gen(rd()); // Mersenne Twister PRNG
	// std::uniform_int_distribution<int> dist(0, 255); // Range: [1, 100]

	// std::string test;
	// int	    sz = 5000000;
	// test.reserve(sz);
	// for (int i = 0; i < sz; i++) {
	// 	int idx = dist(gen);
	// 	while (!scanner.keychars[idx]) {
	// 		idx = dist(gen);
	// 	}
	// 	test.push_back(idx);
	// }
	// std::ofstream file("that.txt");
	// file << test;
	// file.close();

	// Pretend parser is calling scanner
	// while (scanner.printNextToken()) {}

	Compiler compiler{ argc, argv };
	compiler.run();

	// print_ast(nullptr);

	return 0;
}
