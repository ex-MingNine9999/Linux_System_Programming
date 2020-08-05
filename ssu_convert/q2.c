
#include <stdio.h>

#include "Stack.h"


int main(int argc, char* argv[])
{
	
	Stack st;
	newStack(&st);
	
	push(&st, 5);
	push(&st, 2);
	push(&st, 3);
	push(&st, 4);
	push(&st, 1);
	
	printStack(&st);
	
	pop(&st);
	pop(&st);
	push(&st, 15);
	
	printf("TOP OF STACK : %d\n", peek(&st));
	
	printStack(&st);
	
	pop(&st);
	pop(&st);
	pop(&st);
	pop(&st);
	
	push(&st, 30);
	
	printStack(&st);

	return 0;
}
