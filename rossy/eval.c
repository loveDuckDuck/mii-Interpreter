#include "eval.h"

#define LASSERT(args, cond, err) \
    if (!(cond))                 \
    {                            \
        lval_del(args);          \
        return lval_err(err);    \
    }

lval *lval_read_num(mpc_ast_t *t)
{
    errno = 0;
    float x = strtof(t->contents, NULL);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval *lval_eval(lval *v)
{
    /* Evaluate Sexpressions */
    if (v->type == LVAL_SEXPR)
    {
        return lval_eval_sexpr(v);
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

lval *lval_eval_sexpr(lval *v)
{
    /* Evaluate Children */
    for (int i = 0; i < v->count; ++i)
    {
        v->cell[i] = lval_eval(v->cell[i]);
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
    if (f->type != LVAL_SYM)
    {
        lval_del(f);
        lval_del(v);
        return lval_err("S-expression Does not start with symbol!");
    }

    /* Call builtin with operator */
    lval *result = builtin(v, f->sym);
    lval_del(f);
    return result;
}

lval *lval_read(mpc_ast_t *t)
{
    printf("tag : %s - children_num : %d - contents : %s\n", t->tag,t->children_num,t->contents);
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

lval *builtin(lval *a, char *func)
{
    if (strcmp("list", func) == 0)
    {
        return builtin_list(a);
    }
    if (strcmp("head", func) == 0)
    {
        return builtin_head(a);
    }
    if (strcmp("tail", func) == 0)
    {
        return builtin_tail(a);
    }
    if (strcmp("join", func) == 0)
    {
        return builtin_join(a);
    }
    if (strcmp("cons", func) == 0)
    {
        return builtin_cons(a);
    }
    if (strcmp("len", func) == 0)
    {
        return builtin_len(a);
    }
    if (strcmp("eval", func) == 0)
    {
        return builtin_eval(a);
    }
    if (strstr("+-/*", func))
    {
        return builtin_op(a, func);
    }
    lval_del(a);
    return lval_err("Unknown Function!");
}

lval *builtin_op(lval *a, char *op)
{

    /* Ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++)
    {
        if (a->cell[i]->type != LVAL_NUM)
        {
            lval_del(a);
            return lval_err("Cannot operate on non-number!");
        }
    }

    /* Pop the first element */
    lval *x = lval_pop(a, 0);

    /* If no arguments and sub then perform unary negation */
    if ((strcmp(op, "-") == 0) && a->count == 0)
    {
        x->num = -x->num;
    }

    /* While there are still elements remaining */
    while (a->count > 0)
    {

        /* Pop the next element */
        lval *y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0)
        {
            x->num += y->num;
        }
        if (strcmp(op, "-") == 0)
        {
            x->num -= y->num;
        }
        if (strcmp(op, "*") == 0)
        {
            x->num *= y->num;
        }
        if (strcmp(op, "/") == 0)
        {
            if (y->num == 0)
            {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division By Zero!");
                break;
            }
            x->num /= y->num;
        }
        if (strcmp(op, "%") == 0)
        {
            if (y->num == 0)
            {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division By Zero!");
                break;
            }
            int mod = (int)x->num % (int)y->num;
            x->num = mod;
        }
        if (strcmp(op, "^") == 0)
        {
            if (y->num == 0)
            {
                x->num = 1;
            }
            else
            {
                int pow = 1;
                while (y->num > 0)
                {
                    pow = x->num * pow;
                    y->num--;
                }
                x->num = pow;
            }
        }
        if (strcmp(op, "min") == 0)
        {
            return x->num < y->num ? x : y;
        }
        if (strcmp(op, "max") == 0)
        {
            return x->num > y->num ? x : y;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval *builtin_head(lval *a)
{
    /* Check Error Conditions */
    LASSERT(a, a->count == 1, "Function 'head' passed too many arguments!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'head' passed incorrect types!");
    LASSERT(a, a->cell[0]->type != 0, "Function 'head' passed {}!");

    /* Otherwise take first argument */
    lval *v = lval_take(a, 0);

    /* Delete all elements that are not head and return */
    while (v->count > 1)
    {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval *builtin_tail(lval *a)
{
    LASSERT(a, a->count == 1,
            "Function 'tail' passed too many arguments!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
            "Function 'tail' passed incorrect type!");
    LASSERT(a, a->cell[0]->count != 0,
            "Function 'tail' passed {}!");

    lval *v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval *builtin_list(lval *a)
{
    a->type = LVAL_QEXPR;
    return a;
}
lval *builtin_eval(lval *a)
{
    LASSERT(a, a->count == 1,
            "Function 'eval' passed too many arguments!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
            "Function 'eval' passed incorrect type!");

    lval *x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(x);
}

lval *builtin_join(lval *a)
{
    for (int i = 0; i < a->count; ++i)
    {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type.");
    }
    lval *x = lval_pop(a, 0);
    while (a->count)
    {
        x = lval_join(x, lval_pop(a, 0));
    }
    lval_del(a);
    return x;
}

lval *builtin_cons(lval *a)
{
    // Verifica numero argomenti (2)
    LASSERT(a, a->count == 2, "Function 'cons' passed wrong number of args!");
    // Verifica che il secondo argomento sia una Q-Expression
    LASSERT(a, a->cell[1]->type == LVAL_QEXPR, "Function 'cons' second argument must be a Q-Expression!");

    // Estrae il valore da inserire (primo argomento)
    lval *val = lval_pop(a, 0);
    // Estrae la lista target (secondo argomento, ora diventato indice 0 dopo il pop)
    lval *q = lval_pop(a, 0);

    // Elimina il contenitore degli argomenti
    lval_del(a);

    // Logica di Prepend (inserimento in testa)
    // 1. Incrementa il contatore
    q->count++;
    // 2. Rialloca la memoria per ospitare il nuovo elemento
    q->cell = realloc(q->cell, sizeof(lval*) * q->count);
    // 3. Sposta tutti gli elementi esistenti in avanti di una posizione
    //    Sposta da &cell[0] a &cell[1] per (count-1) elementi
    memmove(&q->cell[1], &q->cell[0], sizeof(lval*) * (q->count - 1));
    // 4. Inserisce il nuovo valore in testa
    q->cell[0] = val;

    return q;
}

lval *builtin_len(lval *a)
{
    // Verifica che ci sia esattamente 1 argomento
    LASSERT(a, a->count == 1, "Function 'len' passed too many arguments!");
    // Verifica che l'argomento sia una Q-Expression
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'len' passed incorrect type!");

    // Estrae la Q-Expression
    lval *q = lval_pop(a, 0);
    
    // Elimina il contenitore degli argomenti 'a' (non serve più)
    lval_del(a);

    // Salva il conteggio
    long count = q->count;

    // Elimina la Q-Expression (abbiamo estratto l'informazione che ci serviva)
    lval_del(q);

    // Restituisce il conteggio come nuovo lval numerico
    return lval_num((float)count);

}

lval *lval_join(lval *x, lval *y)
{
    while (y->count)
    {
        x = lval_add(x, lval_pop(y, 0));
    }
    lval_del(y);
    return x;
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

lval *lval_qexpr(void)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
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
    case LVAL_QEXPR:
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
    }
}
void lval_println(lval *v)
{
    lval_print(v);
    putchar('\n');
}