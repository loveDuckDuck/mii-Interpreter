#ifndef EVAL_H
#define EVAL_H

#include <stdio.h>
#include "mpc.h"

/* 
    Declare New lval Struct 
    lval = LispValue
*/

typedef struct {
  int type;
  long num;
  /* Error and Symbol types have some string data */
  char* err;
  char* sym;
  /* Count and Pointer to a list of "lval*" */
  int count;
  struct lval** cell;
} lval;

/* Create Enumeration of Possible lval Types */
enum LVAL_TYPES{ LVAL_NUM, LVAL_ERR ,LVAL_SYM, LVAL_SEXPR};

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

/* Delete an "lval" */
void lval_del(lval *v);


/* Print an "lval" */
void lval_print(lval v);
/* Print an "lval" followed by a newline */
void lval_println(lval v);

#endif