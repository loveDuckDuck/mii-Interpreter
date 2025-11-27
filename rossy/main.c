#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#include "eval.h"

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
	mpc_parser_t *Expr = mpc_new("expr");
	mpc_parser_t *Symbol = mpc_new("symbol");
	mpc_parser_t *Sexpr = mpc_new("sexpr");
	mpc_parser_t *Qexpr = mpc_new("qexpr");
	mpc_parser_t *Rossy = mpc_new("rossy");

	mpca_lang(MPCA_LANG_DEFAULT,
			  "                               				\
	integer 	:/-?[0-9]+/ ;                     			\
	float 		: /-?[0-9]+\\.[0-9]+/ ;						\
    number   	:  <float> | <integer> ;              		\
    symbol 		: '+' | '-' | '*' | '/' | '%' | '^'			\
				| \"list\" | \"head\" | \"tail\" | \"join\" \
				| \"eval\" | \"cons\"|\"len\"|\"min\" |  \"max\";  			\
	qexpr 		: '{' <expr>* '}'		;					\
	sexpr 		: '(' <expr>* ')'		;					\
    expr     	: <number> | <symbol> | <sexpr> | <qexpr> ;  		\
    rossy    	: /^/ <expr>* /$/ ;             			\
  	",
			  Integer, Float, Number, Symbol, Sexpr,Qexpr, Expr, Rossy);
	
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
		if (mpc_parse("<stdin>", input, Rossy, &r))
		{
			lval* x =  lval_eval( lval_read(r.output));
			lval_println(x);
			lval_del(x);
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
	mpc_cleanup(7, Integer, Float, Number, Symbol, Sexpr,Qexpr, Expr, Rossy);
	return 0;
}
