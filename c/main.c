#include "symvar.c"

void main() {
  struct symvar *v, *w;
  struct symconstrs* ccs = NULL;
  v=sv_init_v(ccs,2.0,"Two");
  w=sv_init_d(ccs,1.0);
  sv_print(v);
  sv_print(w);
  v = sv_ss(ccs,ADD,v,w);
  if(sv_constr(&ccs,v,GT,w)) {
    v = sv_sd(ccs,EXP,v,3.0);
  }
  sv_constr_pop(&ccs);
  v = sv_sd(ccs,SUB,v,2.0);
  sv_print(v);
  return;
}
