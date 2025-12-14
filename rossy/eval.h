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
    /* Basic */
    float num;
    char *err;
    char *sym;

    /* Function */
    lbuiltin builtin;
    // where i store it in my enviorament
    lenv *env;
    // list of arguments which our function take in input
    lval *formals;
    // structure of our function, literraly what it do
    lval *body;

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



#define LASSERT_TYPE(func, args, index, expect)                     \
    LASSERT(args, args->cell[index]->type == expect,                \
            "Function '%s' passed incorrect type for argument %i. " \
            "Got %s, Expected %s.",                                 \
            func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num)                               \
    LASSERT(args, args->count == num,                              \
            "Function '%s' passed incorrect number of arguments. " \
            "Got %i, Expected %i.",                                \
            func, args->count, num)                 

#define LASSERT_NOT_EMPTY(func, args, index)     \
    LASSERT(args, args->cell[index]->count != 0, \
            "Function '%s' passed {} for argument %i.", func, index);
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

/* Create our lambda function*/
lval *lval_lambda(lval *formals, lval *body);

lval *lval_call(lenv *e, lval *f, lval *a);

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

char *ltype_name(int t);

/*use this forward declaration to avoid to get the circular dependency*/
lval *lenv_get(lenv *e, lval *k);
lenv *lenv_new(void);
void lenv_del(lenv *e);
lenv *lenv_copy(lenv *e);
void lenv_put(lenv *e, lval *k, lval *v);
void lenv_def(lenv *e, lval *k, lval *v);

#endif