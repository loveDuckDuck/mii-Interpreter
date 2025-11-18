#include "eval.h"

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