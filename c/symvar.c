#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum { DOUBLE, INT, VAR, ADD, SUB, MUL, DIV, EXP } symexp_op;
typedef enum { EQ, LTEQ, GTEQ, LT, GT } symconstr_op;

struct symexp {
	symexp_op op;
	union {
		double constd;
		int consti;
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
		symconstr_op op;
		struct symexp* left;
		struct symexp* right;
	} *exp;
};

struct symvar {
	double nominal;
	struct symexp *exp;
	struct symconstrs *constrs;
};

struct symvar* sv_init_i(int nominal) {
  struct symvar* v = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  v->nominal = (double)nominal;
  v->exp = exp;
  v->constrs = NULL;
  exp->op = INT;
  exp->consti = nominal;
  return v;
}

struct symvar* sv_init_d(double nominal) {
  struct symvar* v = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  v->nominal = nominal;
  v->exp = exp;
  v->constrs = NULL;
  exp->op = DOUBLE;
  exp->constd = nominal;
  return v;
}

struct symvar* sv_ss(symexp_op op, struct symvar* l, struct symvar* r) {
  struct symvar* res = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  struct symconstrs* constrs = malloc(sizeof(struct symconstrs));
  res->constrs = constrs;
  res->exp = exp;
  constrs->left = l->constrs;
  constrs->right = r->constrs;
  constrs->exp = NULL;
  exp->left = l->exp;
  exp->right = r->exp;
  exp->op = op;
  switch(op) {
    case ADD:
      res->nominal = l->nominal + r->nominal;
      break;
    case SUB:
      res->nominal = l->nominal - r->nominal;
      break;
    case MUL:
      res->nominal = l->nominal * r->nominal;
      break;
    case DIV:
      res->nominal = l->nominal / r->nominal;
      break;
    case EXP:
      res->nominal = pow(l->nominal, r->nominal);
      break;
  }
  return res;
}

struct symvar* sv_sd(symexp_op op, struct symvar* l, double r) {
  struct symvar* res = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  struct symexp* expr = malloc(sizeof(struct symexp));
  res->constrs = l->constrs;
  res->exp = exp;
  exp->left = l->exp;
  exp->right = expr;
  exp->op = op;
  expr->op = DOUBLE;
  expr->constd = r;
  switch(op) {
    case ADD:
      res->nominal = l->nominal + r;
      break;
    case SUB:
      res->nominal = l->nominal - r;
      break;
    case MUL:
      res->nominal = l->nominal * r;
      break;
    case DIV:
      res->nominal = l->nominal / r;
      break;
    case EXP:
      res->nominal = pow(l->nominal, r);
      break;
  }
  return res;
}

void sv_print_exp(struct symexp* e) {
  if(e==NULL) return;
  switch(e->op) {
    case DOUBLE:
      printf("%lf",e->constd);
      return;
    case INT:
      printf("%d",e->consti);
      return;
    case VAR:
      printf("%s",e->var);
      return;
    case ADD:
      printf("(");
      sv_print_exp(e->left);
      printf(")+(");
      sv_print_exp(e->right);
      printf(")");
      break;
    case SUB:
      printf("(");
      sv_print_exp(e->left);
      printf(")-(");
      sv_print_exp(e->right);
      printf(")");
      break;
    case MUL:
      printf("(");
      sv_print_exp(e->left);
      printf(")*(");
      sv_print_exp(e->right);
      printf(")");
      break;
    case DIV:
      printf("(");
      sv_print_exp(e->left);
      printf(")/(");
      sv_print_exp(e->right);
      printf(")");
      break;
    case EXP:
      printf("(");
      sv_print_exp(e->left);
      printf(")^(");
      sv_print_exp(e->right);
      printf(")");
      break;
  }
  return;
}

void sv_print_constrs(struct symconstrs* c) {
  if(c==NULL) return;
  if(c->exp) {
    printf("\n    ");
    sv_print_exp(c->exp->left);
    switch(c->exp->op) {
      case EQ:
        printf(" = ");
        break;
      case LTEQ:
        printf(" <= ");
        break;
      case GTEQ:
        printf(" >= ");
        break;
      case LT:
        printf(" < ");
        break;
      case GT:
        printf(" > ");
        break;
    }
    sv_print_exp(c->exp->right);
  }
  sv_print_constrs(c->left);
  sv_print_constrs(c->right);
}

void sv_print(struct symvar* v) {
  printf("NOMINAL:  %lf\n",v->nominal);
  printf("EXPR:     ");
  sv_print_exp(v->exp);
  printf("\nCONSTRS:  ");
  sv_print_constrs(v->constrs);
  printf("\n");
}
