#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum { DOUBLE, INT, VAR, ADD, SUB, MUL, DIV, EXP } symexp_op;
typedef enum { EQ, NEQ, LTEQ, GT, LT, GTEQ } symconstr_op;
//Recommended not to use EQ/NEQ as nominal values are floating-point

struct symexp {
	symexp_op op;
  struct symexp* left;
  struct symexp* right;
  char* var;
  double constd;
  int consti;
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

struct symconstrs* sv_copyccs(struct symconstrs* ccs) {
  if(ccs == NULL) return NULL;
  struct symconstrs* copy = malloc(sizeof(struct symconstrs));
  copy->exp = ccs->exp;
  copy->left = sv_copyccs(ccs->left);
  copy->right = sv_copyccs(ccs->right);
  return copy;
}

void sv_constr_pop(struct symconstrs** ccs) {
  struct symconstrs* old_ccs = *ccs;
  *ccs = (*ccs)->right;
  free(old_ccs);
}

int sv_constr(struct symconstrs** ccs, struct symvar* l, symconstr_op op, struct symvar* r) {
  int res;
  double lv=l->nominal, rv=r->nominal;
  switch(op) {
    case EQ:
      if(lv==rv)
        res=1;
      else
        res=0;
      break;
    case LT:
      if(lv<rv)
        res=1;
      else
        res=0;
      break;
    case GT:
      if(lv>rv)
        res=1;
      else
        res=0;
      break;
    case NEQ:
      if(lv==rv)
        res=0;
      else
        res=0;
      break;
    case GTEQ:
      if(lv<rv)
        res=0;
      else
        res=1;
      break;
    case LTEQ:
      if(lv>rv)
        res=0;
      else
        res=1;
      break;
  }
  struct symconstrs* ccs_old = *ccs;
  *ccs = malloc(sizeof(struct symconstrs));
  (*ccs)->right=ccs_old;
  (*ccs)->left=NULL;
  (*ccs)->exp = malloc(sizeof(struct symconstr));
  (*ccs)->exp->left = l->exp;
  (*ccs)->exp->right = r->exp;
  (*ccs)->exp->op = op^(1-res);
  return res;
}

struct symvar* sv_init_i(struct symconstrs* ccs, int nominal) {
  struct symvar* v = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  v->nominal = (double)nominal;
  v->exp = exp;
  v->constrs = ccs;
  exp->op = INT;
  exp->consti = nominal;
  return v;
}

struct symvar* sv_init_d(struct symconstrs* ccs, double nominal) {
  struct symvar* v = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  v->nominal = nominal;
  v->exp = exp;
  v->constrs = ccs;
  exp->op = DOUBLE;
  exp->constd = nominal;
  return v;
}

struct symvar* sv_init_v(struct symconstrs* ccs, double nominal, char* vn) {
  struct symvar* v = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  v->nominal = nominal;
  v->exp = exp;
  v->constrs = ccs;
  exp->op = VAR;
  exp->constd = nominal;
  exp->var = vn;
  return v;
}

struct symvar* sv_ss(struct symconstrs* ccs, struct symvar* l, symexp_op op, struct symvar* r) {
  struct symvar* res = malloc(sizeof(struct symvar));
  struct symexp* exp = malloc(sizeof(struct symexp));
  struct symconstrs* constrs = malloc(sizeof(struct symconstrs));
  res->exp = exp;
  constrs->left = l->constrs;
  constrs->right = r->constrs;
  constrs->exp = NULL;
  if(ccs == NULL) {
    res->constrs = constrs;
  } else {
    res->constrs = sv_copyccs(ccs);
    res->constrs->left = constrs;
  }
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

struct symvar* sv_sd(struct symconstrs* ccs, struct symvar* l, symexp_op op, double rd) {
  struct symvar* r = sv_init_d(NULL, rd);
  return sv_ss(ccs, l, op, r);
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
      case NEQ:
        printf(" != ");
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
