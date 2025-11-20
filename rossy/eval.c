#include "eval.h"

lval *lval_read_num(mpc_ast_t *t)
{
    errno = 0;
    float x = strtof(t->contents, NULL);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

/*average search on a tree*/
lval eval(mpc_ast_t *t)
{
    /* If tagged as number return it directly. */
    if (strstr(t->tag, "number"))
    {
        errno = 0;
        float val = strtof(t->contents, NULL);
        return errno != ERANGE ? lval_num(val) : lval_err(LERR_BAD_NUM);
    }

    /* The operator is always second child. */
    char *op = t->children[1]->contents;

    /* We store the third child in `x` */
    lval x = eval(t->children[2]);

    /* Iterate the remaining children and combining. */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr"))
    {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    return x;
}

lval *lval_read(mpc_ast_t *t)
{

    /* If Symbol or Number return conversion to that type */
    if (strstr(t->tag, "number"))
    {
        return lval_read_num(t);
    }
    if (strstr(t->tag, "symbol"))
    {
        return lval_sym(t->contents);
    }

    /* If root (>) or sexpr then create empty list */
    lval *x = NULL;
    if (strcmp(t->tag, ">") == 0)
    {
        x = lval_sexpr();
    }
    if (strstr(t->tag, "sexpr"))
    {
        x = lval_sexpr();
    }

    /* Fill this list with any valid expression contained within */
    for (int i = 0; i < t->children_num; i++)
    {
        if (strcmp(t->children[i]->contents, "(") == 0 ||
            strcmp(t->children[i]->contents, ")") == 0 ||
            strcmp(t->children[i]->tag, "regex") == 0)
        {
            continue;
        }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

lval *lval_add(lval *v, lval *x)
{
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval *) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

void lval_expr_print(lval *v, char open, char close)
{
    putchar(open);
    for (int i = 0; i < v->count; i++)
    {

        /* Print Value contained within */
        lval_print(v->cell[i]);

        /* Don't print trailing space if last element */
        if (i != (v->count - 1))
        {
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_print(lval* v) {
  switch (v->type) {
    case LVAL_NUM:   printf("%li", v->num); break;
    case LVAL_ERR:   printf("Error: %s", v->err); break;
    case LVAL_SYM:   printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
  }
}

void lval_println(lval* v) { lval_print(v); putchar('\n'); }

/* Use operator string to see which operation to perform */
lval eval_op(lval x, char *op, lval y)
{
    // if one of the two operator are errors return
    if (x.err == LVAL_ERR)
    {
        return x;
    }
    if (y.err == LVAL_ERR)
    {
        return x;
    }

    if (strcmp(op, "+") == 0)
    {
        return lval_num(x.num + y.num);
    }
    if (strcmp(op, "-") == 0)
    {
        return lval_num(x.num - y.num);
    }
    if (strcmp(op, "*") == 0)
    {
        return lval_num(x.num * y.num);
    }
    if (strcmp(op, "/") == 0)
    {
        return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
    }
    if (strcmp(op, "%") == 0)
    {
        return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num((int)x.num % (int)y.num);
    }
    if (strcmp(op, "^") == 0)
    {
        if (y.num == 0)
        {
            return lval_num(1);
        }
        else
        {
            int pow = 1;
            while (y.num > 0)
            {
                pow = x.num * pow;
                y.num--;
            }
            return lval_num(pow);
        }
    }
    if (strcmp(op, "min") == 0)
    {
        return x.num < y.num ? x : y;
    }
    if (strcmp(op, "max") == 0)
    {
        return x.num > y.num ? x : y;
    }

    return lval_err(LERR_BAD_OP);
}

/* Create a new number type lval */
lval *lval_num(float x)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

/* Create a new error type lval */
lval *lval_err(char *m)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}
lval *lval_sym(char *s)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval *lval_sexpr(void)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void lval_del(lval *v)
{
    /*
    Here im gone delete all the mmemoery allocated on the struct
    avoding the memeory leak, so in  the case of the number i dont need to free anything
    in the case of error and symbol i need to free the string allocated
    in the case of sexpr i need to free all the cell allocated and then the cell pointer itself
    */
    switch (v->type)
    {
    case LVAL_NUM:

        break;

    case LVAL_ERR:
        free(v->err);
        break;

    case LVAL_SYM:
        free(v->sym);
        break;

    case LVAL_SEXPR:
        for (int i = 0; i < v->count; i++)
        {
            lval_del(v->cell[i]);
        }
        free(v->cell);
        break;
    }
    /*Finally free the lval struct itself*/
    free(v);
}

/* Print an "lval" */
void lval_print(lval v)
{
    switch (v.type)
    {
    /* In the case the type is a number print it */
    /* Then 'break' out of the switch. */
    case LVAL_NUM:
        printf("%.2f", v.num);
        break;

    /* In the case the type is an error */
    case LVAL_ERR:
        /* Check what type of error it is and print it */
        if (v.err == LERR_DIV_ZERO)
        {
            puts("Error: Division By Zero!");
        }
        if (v.err == LERR_BAD_OP)
        {
            puts("Error: Invalid Operator!");
        }
        if (v.err == LERR_BAD_NUM)
        {
            puts("Error: Invalid Number!");
        }
        break;
    }
}

/* Print an "lval" followed by a newline */
void lval_println(lval v)
{
    lval_print(v);
}