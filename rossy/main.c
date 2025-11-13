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
			  "                               		\
	integer :/-?[0-9]+/ ;                     		\
	float : /-?[0-9]*.[0-9]+/ ;						\
    number   :  <float> | <integer> ;              	\
	operator : '+' | '-' | '*' | '/' ;          	\
    factor   : <number> | '(' <expr> ')' ;      	\
    term     : <factor> (('*' | '/') <factor>)* ; 	\
    expr     : <term> (('+' | '-') <term>)* ;   	\
    rossy    : /^/ <expr> /$/ ;                 	\
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
			/*On success print the AST*/
			mpc_ast_print(r.output);
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
