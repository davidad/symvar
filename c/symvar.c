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
	struct symconstrs *left;
	struct symconstrs *right;
	struct symconstr {
		enum { EQ, LTEQ, GTEQ, LT, GT } op;
		struct symexp* exp;
	};
};

struct symvar {
	double nominal;
	struct symexp *exp;
	struct symconsts *constrs;
};

struct symvar sv_add_ss(struct symvar l, struct symvar r) {
  struct symvar* res = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  struct symexp* constrs = malloc(sizeof(struct symconstrs));
  res.nominal = l.nominal + r.nominal;
  res.constrs = constrs;
  res.exp = exp;
  constrs.left = l.constrs;
  eøp.right = r.constrs;
  exp.left = l.exp;
  eøp.right = r.exp;
  exp.op = ADD;
}
