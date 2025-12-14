#include "env.h"

lenv *lenv_new(void)
{
    lenv *env = malloc(sizeof(lenv));
    env->count = 0;
    env->par = NULL;
    env->syms = NULL;
    env->vals = NULL;

    return env;
}
void lenv_del(lenv *e)
{
    for (int i = 0; i < e->count; ++i)
    {
        lval_del(e->vals[i]);
        free(e->syms[i]);
    }
    free(e->syms);
    free(e->vals);

    free(e);
}

lval *lenv_get(lenv *e, lval *k)
{
    for (int i = 0; i < e->count; ++i)
    {
        if (strcmp(e->syms[i], k->sym) == 0)
            return lval_copy(e->vals[i]);
    }
    if (e->par)
    {
        return lenv_get(e->par, k);
    }
    else
    {
        /* If no symbol found return error */
        return lval_err("Unbound Symbol '%s'", k->sym);
    }
}
void lenv_put(lenv *e, lval *k, lval *v)
{
    /*check if  the values exist*/
    for (int i = 0; i < e->count; ++i)
    {
        if (strcmp(e->syms[i], k->sym) == 0)
        {
            /*if exist delete the all value and replace
            with the new one*/
            lval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }

    /*realloc the size of the enviorament*/
    e->count++;
    e->vals = realloc(e->vals, sizeof(lval *) * e->count);
    e->syms = realloc(e->syms, sizeof(char *) * e->count);

    /* Copy contents of lval and symbol string into new location */
    e->vals[e->count - 1] = lval_copy(v);
    e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
    strcpy(e->syms[e->count - 1], k->sym);
}
void lenv_def(lenv *e, lval *k, lval *v)
{ /* Iterate till e has no parent */
    while (e->par)
    {
        e = e->par;
    }
    lenv_put(e, k, v);
}
lenv *lenv_copy(lenv *e)
{
    lenv *n = malloc(sizeof(lenv));
    // n->par = e->par;
    n->count = e->count;
    n->par = e->par;
    n->syms = malloc(sizeof(char *) * n->count);
    n->vals = malloc(sizeof(lval *) * n->count);
    for (int i = 0; i < e->count; i++)
    {
        n->syms[i] = malloc(strlen(e->syms[i]) + 1);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = lval_copy(e->vals[i]);
    }
    return n;
}

void lenv_add_builtin(lenv *e, char *name, lbuiltin func)
{
    lval *k = lval_sym(name);
    lval *v = lval_fun(func);
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}
lval *builtin_exit(lenv *e, lval *a)
{
    // lenv_del(e);
    // lval_del(a);

    printf("go out");
    exit(EXIT_SUCCESS);
    return NULL;
}

void lenv_add_builtins(lenv *e)
{
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "len", builtin_len);
    lenv_add_builtin(e, "eval", builtin_eval);

    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "\\", builtin_lambda);
    lenv_add_builtin(e, "=", builtin_put);

    lenv_add_builtin(e, "exit", builtin_exit);
}

/*
===================
IMPLEMENT THE BUILTIN FUNCTION
===================
*/

lval *builtin(lenv *e, lval *a, char *func)
{
    if (strcmp("list", func) == 0)
    {
        return builtin_list(e, a);
    }
    if (strcmp("head", func) == 0)
    {
        return builtin_head(e, a);
    }
    if (strcmp("tail", func) == 0)
    {
        return builtin_tail(e, a);
    }
    if (strcmp("join", func) == 0)
    {
        return builtin_join(e, a);
    }
    if (strcmp("cons", func) == 0)
    {
        return builtin_cons(e, a);
    }
    if (strcmp("len", func) == 0)
    {
        return builtin_len(e, a);
    }
    if (strcmp("eval", func) == 0)
    {
        return builtin_eval(e, a);
    }
    if (strstr("+-/*", func))
    {
        return builtin_op(e, a, func);
    }
    lval_del(a);
    return lval_err("Unknown Function!");
}

lval *builtin_op(lenv *e, lval *a, char *op)
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

lval *builtin_add(lenv *e, lval *a)
{
    return builtin_op(e, a, "+");
}

lval *builtin_sub(lenv *e, lval *a)
{
    return builtin_op(e, a, "-");
}

lval *builtin_mul(lenv *e, lval *a)
{
    return builtin_op(e, a, "*");
}

lval *builtin_div(lenv *e, lval *a)
{
    return builtin_op(e, a, "/");
}

lval *builtin_head(lenv *e, lval *a)
{
    /* Check Error Conditions */
    LASSERT(a, a->count == 1, "Function 'head' passed too many arguments!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'head' passed incorrect types!",
            "Got %s , Expected",
            ltype_name(a->cell[0]->type),
            ltype_name(a->cell[1]->type));
    LASSERT(a, a->cell[0]->type != 0, ltype_name(LVAL_QEXPR));

    /* Otherwise take first argument */
    lval *v = lval_take(a, 0);

    /* Delete all elements that are not head and return */
    while (v->count > 1)
    {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval *builtin_tail(lenv *e, lval *a)
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

lval *builtin_list(lenv *e, lval *a)
{
    a->type = LVAL_QEXPR;
    return a;
}
lval *builtin_eval(lenv *e, lval *a)
{
    LASSERT(a, a->count == 1,
            "Function 'eval' passed too many arguments!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
            "Function 'eval' passed incorrect type!");

    lval *x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval *builtin_join(lenv *e, lval *a)
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

lval *builtin_cons(lenv *e, lval *a)
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
    q->cell = realloc(q->cell, sizeof(lval *) * q->count);
    // 3. Sposta tutti gli elementi esistenti in avanti di una posizione
    //    Sposta da &cell[0] a &cell[1] per (count-1) elementi
    memmove(&q->cell[1], &q->cell[0], sizeof(lval *) * (q->count - 1));
    // 4. Inserisce il nuovo valore in testa
    q->cell[0] = val;

    return q;
}

lval *builtin_len(lenv *e, lval *a)
{
    LASSERT(a, a->count == 1, "Function 'len' passed too many arguments!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'len' passed incorrect type!");

    lval *q = lval_pop(a, 0);

    lval_del(a);

    long count = q->count;

    lval_del(q);

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

lval *builtin_def(lenv *e, lval *v)
{
    return builtin_var(e, v, true);
}

lval *builtin_put(lenv *e, lval *v)
{
    return builtin_var(e, v, false);
}

lval *builtin_var(lenv *e, lval *a, bool is_global_func)
{
    printf("%d\n",a->cell[0]->type);
    // LASSERT_TYPE(func, a, 0, LVAL_QEXPR);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
            "Function 'def' passed incorrect type!");

    lval *syms = a->cell[0];

    /*ensure all elements of first list are symbols*/
    for (int i = 0; i < syms->count; ++i)
    {
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "Function '%s' cannot define non-symbol. "
                "Got %s, Expected %s.",
                is_global_func ? "def" : "put",
                ltype_name(syms->cell[i]->type),
                ltype_name(LVAL_SYM));
    }
    /* Check correct number of symbols and values */
    LASSERT(a, (syms->count == a->count - 1),
            "Function '%s' passed too many arguments for symbols. "
            "Got %i, Expected %i.",
            is_global_func ? "def" : "put", syms->count, a->count - 1);

    /* Assign copies of values to symbols */
    for (int i = 0; i < syms->count; i++)
    {
        if (is_global_func)
            lenv_def(e, syms->cell[i], a->cell[i + 1]);
        else
            lenv_put(e, syms->cell[i], a->cell[i + 1]);
    }

    lval_del(a);
    return lval_sexpr();
}

lval *builtin_lambda(lenv *e, lval *v)
{
    LASSERT_NUM("\\", v, 2);
    LASSERT_TYPE("\\", v, 0, LVAL_QEXPR);
    LASSERT_TYPE("\\", v, 1, LVAL_QEXPR);

    /* Check first Q-Expression contains only Symbols */
    for (int i = 0; i < v->cell[0]->count; i++)
    {
        LASSERT(v, (v->cell[0]->cell[i]->type == LVAL_SYM),
                "Cannot define non-symbol. Got %s, Expected %s.",
                ltype_name(v->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
    }
    /* Pop first two arguments and pass them to lval_lambda */
    lval *formals = lval_pop(v, 0);
    lval *body = lval_pop(v, 0);
    lval_del(v);

    return lval_lambda(formals, body);
}

lval *lval_call(lenv *e, lval *f, lval *a)
{
    if (f->builtin)
    {
        return f->builtin(e, a);
    }

    /*record argumento counts*/
    int given = a->count;
    int total = f->formals->count;

    while (a->count)
    {

        if (f->formals->count == 0)
        {
            lval_del(a);
            return lval_err(
                "Function passed too many arguments. "
                "Got %i, Expected %i.",
                given, total);
        }

        /*pop the first sumbo from the formals*/
        lval *sym = lval_pop(f->formals, 0);
        /* Pop the next argument from the list */
        lval *val = lval_pop(a, 0);

        if (strcmp(sym->sym, "&") == 0)
        {
            if (f->formals->count != 1)
            {
                lval_del(a);
                return lval_err("Function format invalid. "
                                "Symbol '&' not followed by single symbol.");
            }

            /* Next formal should be bound to remaining arguments */
            lval *nsym = lval_pop(f->formals, 0);

            /* Bind a copy into the function's environment */
            lenv_put(f->env, nsym, builtin_list(e, a));
            /* Delete symbol and value */
            lval_del(sym);
            lval_del(val);
            break;
        }
        /* If '&' remains in formal list bind to empty list */
        if (f->formals->count > 0 &&
            strcmp(f->formals->cell[0]->sym, "&") == 0)
        {

            /* Check to ensure that & is not passed invalidly. */
            if (f->formals->count != 2)
            {
                return lval_err("Function format invalid. "
                                "Symbol '&' not followed by single symbol.");
            }

            /* Pop and delete '&' symbol */
            lval_del(lval_pop(f->formals, 0));

            /* Pop next symbol and create empty list */
            lval *sym = lval_pop(f->formals, 0);
            lval *val = lval_qexpr();

            /* Bind to environment and delete */
            lenv_put(f->env, sym, val);
            lval_del(sym);
            lval_del(val);
        }
    }

    lval_del(a);
    /* If all formals have been bound evaluate */
    if (f->formals->count == 0)
    {
        f->env->par = e;
        return builtin_eval(
            f->env, lval_add(lval_sexpr(), lval_copy(f->body)));
    }
    else
    {
        /* Otherwise return partially evaluated function */
        return lval_copy(f);
    }
}
