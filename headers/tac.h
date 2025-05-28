#pragma once

typedef enum { OP_ASSIGN, OP_PARAM, OP_CALL, OP_RETRIEVE } OpType;

typedef enum { OPERAND_CONST, OPERAND_VAR } OperandType;

typedef struct {
	OperandType type;

	union {
		int   value;
		char *name;
	} u;
} Operand;

typedef struct Quad {
	OpType	     op;
	Operand	     src1, src2, dest;
	struct Quad *next;
} Quad;
