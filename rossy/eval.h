#ifndef EVAL_H
#define EVAL_H

#include <stdio.h>
#include "mpc.h"

/* 
    Declare New lval Struct 
    lval = LispValue
*/

typedef struct lval {
  int type;
  float num;
  char* err;
  char* sym;
  int count;
  struct lval** cell;
} lval;

/* Create Enumeration of Possible lval Types */
enum LVAL_TYPES{ LVAL_NUM, LVAL_ERR ,LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR};

/* Create Enumeration of Possible Error Types */
enum LVAL_TYPES_ERRORS{ LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };


lval eval(mpc_ast_t *t);
lval eval_op(lval x, char *op, lval y);

/* Create a new number type lval */
lval *lval_num(float x);

/* Create a new error type lval */
lval *lval_err(char* m);

/* Create a new symbol type lval */
lval *lval_sym(char *s);

/* Create a new symbol expression type lval */
lval *lval_sexpr(void);
lval *lval_qexpr(void);

/* Delete an "lval" */
void lval_del(lval *v);

/*funciton to evaluteed the sexpression*/
lval *lval_eval_sexpr(lval *v);

lval *builtin_op(lval *a, char *op);

/* Print an "lval" */
void lval_print(lval*  v);
/* Print an "lval" followed by a newline */
void lval_println(lval* v);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);

lval *lval_add(lval *v, lval *x);
lval *lval_read(mpc_ast_t *t);
lval* builtin(lval* a, char* func);

lval *builtin_head(lval *a);
lval *builtin_tail(lval *a);
lval *builtin_list(lval *a);
lval *builtin_join(lval *a);
lval *builtin_cons(lval *a);

lval * lval_join(lval *x, lval*y);
lval *builtin_eval(lval *a);






lval* lval_eval(lval* v) ;
lval * lval_join(lval *x, lval*y);

#endif