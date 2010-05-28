#include <stdio.h>
#include <stdlib.h>

struct symexp {
	enum { CONST, VAR, ADD, SUB, MUL, DIV, EXP } op;
	union {
		double constant;
		struct {
			struct symexp* left;
			struct symexp* right;
		};
		char* var;
	};
};

struct symconstrs {
	struct symconstrs *next;
	struct symconstr {
		enum { EQ, LTEQ, GTEQ, LT, GT } op;
		struct symexp* right;
	};
};

struct symvar {
	double nominal;
	struct symexp *symexp;
	struct symconsts *symconstrs;
};
