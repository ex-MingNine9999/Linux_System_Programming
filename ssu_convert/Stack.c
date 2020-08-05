
#include <stdio.h>
#include <stdlib.h>

#include "Stack.h"

#define STACK_SIZE 10




Stack newStack(Stack *S){
	S->top = -1;
	S->stack = (int *)calloc(STACK_SIZE, sizeof(int));

	return *S;
}

int peek(Stack *S){
	return S->stack[S->top];
}

void push(Stack *S, int value){
	S->stack[++S->top] = value;
	printf("%d PUSH !\n", S->stack[S->top]);
}

int pop(Stack *S){
	printf("%d POP !\n", S->stack[S->top]);
	return S->stack[S->top--];
}

void printStack(Stack *S){
	printf("\n-----STACK LIST-----\n");
	
	for(int i=S->top; i>=0; i--){
		printf("%d\n",S->stack[i]);
	}
	
	printf("-----END OF LIST-----\n");
}
