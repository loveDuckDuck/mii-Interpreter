#ifndef EVAL_TYPES_H
#define EVAL_TYPES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "eval.h"

/*
    Struct lenv
    is a simple struct that allow me to store all the variable and funtion on it
    Each entry in one list has a corresponding entry in the other list at the same position.
    so syms and vals are connected
*/
struct lenv
{
    int count;
    lenv *par; // this referr to the global ambient, so in all my bultin funcion in got an ambient here i got the global one
    char **syms;
    lval **vals;
};

/*
===================
FUNCTION LENV
===================
*/
lenv *lenv_new(void);
void lenv_del(lenv *e);
lenv *lenv_copy(lenv *e);

lval *lenv_get(lenv *e, lval *k);
void lenv_put(lenv *e, lval *k, lval *v);
void lenv_def(lenv *e, lval *k, lval *v);
void lenv_add_builtin(lenv *e, char *name, lbuiltin func);
void lenv_add_builtins(lenv *e);

/*
===================
DEFINE THE BUILTIN FUNCTION
===================
*/
lval *builtin(lenv *e, lval *a, char *func);

lval *builtin_op(lenv *e, lval *a, char *op);

lval *builtin_head(lenv *e, lval *a);
lval *builtin_tail(lenv *e, lval *a);
lval *builtin_list(lenv *e, lval *a);
lval *builtin_join(lenv *e, lval *a);
lval *builtin_cons(lenv *e, lval *a);
lval *builtin_len(lenv *e, lval *a);

lval *builtin_add(lenv *e, lval *a);
lval *builtin_sub(lenv *e, lval *a);
lval *builtin_mul(lenv *e, lval *a);
lval *builtin_div(lenv *e, lval *a);

// If I return a nullptr
lval *builtin_exit(lenv *e, lval *a);

lval *lval_join(lval *x, lval *y);
lval *builtin_eval(lenv *e, lval *a);
/* if is_global_func  is true
so my variable gona be global otherwise gona be loca
global = lenv_def
local = lenv_put
*/

lval *builtin_var(lenv *e, lval *a, bool is_global_func);

lval *builtin_def(lenv *e, lval *a);
lval *builtin_put(lenv *e, lval *a);

lval *builtin_lambda(lenv *e, lval *a);
#endif