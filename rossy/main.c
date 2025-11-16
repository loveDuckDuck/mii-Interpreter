#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

/*If were are on a Windows machine compile these function*/
#ifdef _WIN32
#include <string.h>
static char buffer[2048];
char *readline(char *prompt)
{
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char *cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy) - 1] = '\0';
	return cpy;
}
void add_history(char *unused) {}

#else

/*If we are on a Unix-like system*/
// edit line library
#include <editline/readline.h>
#include <editline/history.h>

#endif
/* Use operator string to see which operation to perform */
float eval_op(float x, char *op, float y)
{
	if (strcmp(op, "+") == 0)
	{
		return x + y;
	}
	if (strcmp(op, "-") == 0)
	{
		return x - y;
	}
	if (strcmp(op, "*") == 0)
	{
		return x * y;
	}
	if (strcmp(op, "/") == 0)
	{
		return x / y;
	}
	if (strcmp(op, "%") == 0)
	{
		return (int)x % (int)y;
	}
	if (strcmp(op, "^") == 0)
	{
		int pow = 1;
		while (y > 0)
		{
			pow = x * pow;
			y--;
		}
		return pow;
	}
	if (strcmp(op, "min") == 0)
	{
		return x < y ? x : y;
	}
	if (strcmp(op, "max") == 0)
	{
		return x > y ? x : y;
	}

	return 0.0;
}

/*average search on a tree*/
float eval(mpc_ast_t *t)
{

	/* If tagged as number return it directly. */
	if (strstr(t->tag, "number"))
	{
		if (strstr(t->tag, "integer"))
			return atoi(t->contents);
		else
			return atof(t->contents);
	}

	/* The operator is always second child. */
	char *op = t->children[1]->contents;

	/* We store the third child in `x` */
	float x = eval(t->children[2]);

	/* Iterate the remaining children and combining. */
	int i = 3;
	while (strstr(t->children[i]->tag, "expr"))
	{
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}

	return x;
}

int main(int argc, char **argv)
{
	mpc_parser_t *Number = mpc_new("number");
	mpc_parser_t *Integer = mpc_new("integer");
	mpc_parser_t *Float = mpc_new("float");
	mpc_parser_t *Operator = mpc_new("operator");
	mpc_parser_t *Expr = mpc_new("expr");
	mpc_parser_t *Factor = mpc_new("factor");
	mpc_parser_t *Term = mpc_new("term");
	mpc_parser_t *Rossy = mpc_new("rossy");

	mpca_lang(MPCA_LANG_DEFAULT,
			  "                               				\
	integer 	:/-?[0-9]+/ ;                     			\
	float 		: /-?[0-9]+\\.[0-9]+/ ;						\
    number   	:  <float> | <integer> ;              		\
    operator : '+' | '-' | '*' | '/' | '%' | '^'| \"min\" |  \"max\";  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  	\
    rossy    : /^/ <operator> <expr>+ /$/ ;             	\
  	",
			  Integer, Float, Number, Operator, Factor, Term, Expr, Rossy);
	puts("Rossy Version 0.0.0.0.3");
	puts("Press Ctrl+C to Exit\n");

	while (1)
	{
		/* Output our prompt */
		char *input = readline("rossy> ");

		/*add inpit to history*/
		add_history(input);
		/* Attempt to Parse the user Input*/
		mpc_result_t r;
		if (mpc_parse("stdin", input, Rossy, &r))
		{
			float result = eval(r.output);
			mpc_ast_print(r.output);

			printf("result>%.2f\n", result);

			mpc_ast_delete(r.output);
		}
		else
		{
			/* Otherwise Print the Error */
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
		free(input);
	}
	/* Undefine and Delete our Parsers */
	mpc_cleanup(8, Integer, Float, Number, Operator, Factor, Term, Expr, Rossy);
	return 0;
}
