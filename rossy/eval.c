#include "eval.h"

/* Create a new number type lval */
lval *lval_num(float x)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

/* Create a new error type lval */
lval *lval_err(char *fmt, ...)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    va_list va;
    va_start(va, fmt);

    v->err = malloc(512);

    vsnprintf(v->err, 511, fmt, va);
    v->err = realloc(v->err, strlen(v->err) + 1);
    va_end(va);

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

lval *lval_qexpr(void)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval *lval_fun(lbuiltin func)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->fun = func;
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
    case LVAL_QEXPR:
    case LVAL_SEXPR:
        for (int i = 0; i < v->count; i++)
        {
            lval_del(v->cell[i]);
        }
        free(v->cell);
        break;
    case LVAL_FUN:
        break;
    }

    /*Finally free the lval struct itself*/
    free(v);
}

void lval_print(lval *v)
{
    switch (v->type)
    {
    case LVAL_NUM:
        printf("%.2f ", v->num);
        break;
    case LVAL_ERR:
        printf("Error: %s", v->err);
        break;
    case LVAL_SYM:
        printf("%s", v->sym);
        break;
    case LVAL_SEXPR:
        lval_expr_print(v, '(', ')');
        break;
    case LVAL_QEXPR:
        lval_expr_print(v, '{', '}');
        break;
    case LVAL_FUN:
        printf("<function>");
        break;
    }
}
void lval_println(lval *v)
{
    lval_print(v);
    putchar('\n');
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

lval *lval_copy(lval *v)
{
    lval *x = malloc(sizeof(lval));
    x->type = v->type;

    switch (v->type)
    {
    case LVAL_NUM:
        x->num = v->num;
        break;
    case LVAL_FUN:
        x->fun = v->fun;
        break;
    case LVAL_SYM:
        x->sym = malloc(strlen(v->sym) + 1);
        strcpy(x->sym, v->sym);
        break;
    case LVAL_SEXPR:
    case LVAL_QEXPR:
        x->count = v->count;
        x->cell = malloc(sizeof(lval *) * x->count);

        for (int i = 0; i < v->count; ++i)
        {
            x->cell[i] = lval_copy(v->cell[i]);
        }
        break;
    case LVAL_ERR:
        x->err = malloc(strlen(v->err) + 1);
        strcpy(x->err, v->err);
        break;
    default:
        break;
    }
    return x;
}

lval *lval_read_num(mpc_ast_t *t)
{
    errno = 0;
    float x = strtof(t->contents, NULL);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval *lval_eval(lenv *e, lval *v)
{

    if (v->type == LVAL_SYM)
    {
        lval *x = lenv_get(e, v);
        lval_del(v);
        return x;
    }

    /* Evaluate Sexpressions */
    if (v->type == LVAL_SEXPR)
    {
        return lval_eval_sexpr(e, v);
    }
    /* All other lval types remain the same */
    return v;
}

lval *lval_pop(lval *v, int i)
{
    /* Find the item at "i" */
    lval *x = v->cell[i];

    /* Shift memory after the item at "i" over the top */
    memmove(&v->cell[i], &v->cell[i + 1],
            sizeof(lval *) * (v->count - i - 1));

    /* Decrease the count of items in the list */
    v->count--;

    /* Reallocate the memory used */
    v->cell = realloc(v->cell, sizeof(lval *) * v->count);
    return x;
}

lval *lval_take(lval *v, int i)
{
    lval *x = lval_pop(v, i);
    lval_del(v);
    return x;
}

lval *lval_eval_sexpr(lenv *e, lval *v)
{
    /* Evaluate Children */
    for (int i = 0; i < v->count; ++i)
    {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }
    /*Emrpty expression*/
    if (v->count == 0)
    {
        return v;
    }
    /* Single Expression */
    if (v->count == 1)
    {
        return lval_take(v, 0);
    }

    /* Ensure First Element is Symbol */
    lval *f = lval_pop(v, 0);
    if (f->type != LVAL_FUN)
    {
        lval_del(f);
        lval_del(v);
        return lval_err("S-expression Does not start with symbol!");
    }

    /* If so call function to get result */
    lval *result = f->fun(e, v);
    lval_del(f);
    return result;
}

lval *lval_read(mpc_ast_t *t)
{
    printf("tag : %s - children_num : %d - contents : %s\n", t->tag, t->children_num, t->contents);
    /* If Symbol or Number return conversion to that type */
    if (strstr(t->tag, "float") || strstr(t->tag, "integer"))
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
    if (strstr(t->tag, "qexpr"))
    {
        x = lval_qexpr();
    }

    /* Fill this list with any valid expression contained within */
    for (int i = 0; i < t->children_num; i++)
    {
        if (strcmp(t->children[i]->contents, "(") == 0 ||
            strcmp(t->children[i]->contents, ")") == 0 ||
            strcmp(t->children[i]->contents, "{") == 0 ||
            strcmp(t->children[i]->contents, "}") == 0 ||
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

char *ltype_name(int t)
{
    switch (t)
    {
    case LVAL_FUN:
        return "Function";
    case LVAL_NUM:
        return "Number";
    case LVAL_ERR:
        return "Error";
    case LVAL_SYM:
        return "Symbol";
    case LVAL_SEXPR:
        return "S-Expression";
    case LVAL_QEXPR:
        return "Q-Expression";
    default:
        return "Unknown";
    }
}
