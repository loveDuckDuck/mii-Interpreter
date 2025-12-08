#ifndef EVAL_H
#define EVAL_H

#include <stdio.h>
#include "mpc.h"

struct lval;
typedef struct lval lval;
struct lenv;
typedef struct lenv lenv;

/*  functr to a funciton with return a lval pointer
    and take in input the enviroaomente and and lval*/
typedef lval *(*lbuiltin)(lenv *, lval *);

/* Create Enumeration of Possible lval Types */
enum LVAL_TYPES
{
    LVAL_NUM,
    LVAL_ERR,
    LVAL_FUN,
    LVAL_SYM,
    LVAL_SEXPR,
    LVAL_QEXPR
};

/* Create Enumeration of Possible Error Types */
enum LVAL_TYPES_ERRORS
{
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM
};

struct lval
{
    int type;

    float num;
    char *err;
    char *sym;
    lbuiltin fun;

    int count;
    struct lval **cell;
};

#define LASSERT(args, cond, fmt, ...)             \
    if (!(cond))                                  \
    {                                             \
        lval *err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args);                           \
        return err;                               \
    }

/*
    Declare New lval Struct
    lval = LispValue
*/
lval eval(mpc_ast_t *t);

/* Create a new number type lval */
lval *lval_num(float x);
/* Create a new error type lval */
lval *lval_err(char *fmt, ...);

/* Create a new symbol type lval */
lval *lval_sym(char *s);

/* Create a new symbol s-expression type lval */
lval *lval_sexpr(void);
/* Create a new symbol q-expression type lval */
lval *lval_qexpr(void);
/* Create a new function pointer*/

lval *lval_fun(lbuiltin func);

lval eval_op(lval x, char *op, lval y);

/*funciton to evaluteed the sexpression*/
lval *lval_eval_sexpr(lenv *e, lval *v);

lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);

lval *lval_add(lval *v, lval *x);
lval *lval_read(mpc_ast_t *t);

lval *lval_eval(lenv *e, lval *v);

/*
===================
UTILS FUNCTION
===================
*/

/* Print an "lval" */
void lval_print(lval *v);
/* Print an "lval" followed by a newline */
void lval_println(lval *v);

/*This function allow me to copy and lval value and return in*/
lval *lval_copy(lval *v);
void lval_expr_print(lval *v, char open, char close);

/* Delete an "lval" */
void lval_del(lval *v);

/*use this forward declaration to avoid to get the circular dependency*/
lval *lenv_get(lenv *e, lval *k);

char* ltype_name(int t);

#endif