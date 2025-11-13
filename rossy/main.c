#include <stdio.h>
#include <stdlib.h>

/*If were are on a Windows machine compile these function*/
#ifdef _WIN32
#include <string.h>
static char buffer[2048];
char* readline(char* prompt){
	fputs(prompt,stdout);
	fgets(buffer,2048,stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy,buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}
void add_history(char* unused){}

#else

/*If we are on a Unix-like system*/
//edit line library
#include <editline/readline.h>
#include <editline/history.h>

#endif


int main (int argc, char **argv){

puts("Rossy Version 0.0.0.0.1");
puts("Press Ctrl+C to Exit\n");


while(1){
	/* Output our prompt */
	char* input = readline("rossy> ");

	/*add inpit to history*/
	add_history(input);
	
	/*Echo the input*/
	printf("Echo %s\n",input);
	free(input);
}

return 0;
}
