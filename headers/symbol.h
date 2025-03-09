#pragma once

// #include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

enum SymbolType { VAR, FUNC };

// class Symbol {
//     private:
// 	const SymbolType symbolType;
// 	std::size_t	 numParams;

//     public:
// 	Symbol(SymbolType);
// 	SymbolType getSymbolType(void) const;

// 	void	    setNumParams(std::size_t);
// 	std::size_t getNumParams(void) const;
// };

typedef struct {
	const enum SymbolType symbolType;
	unsigned int	      numParams;

} Symbol;

#ifdef __cplusplus
}
#endif
