#include <stdlib.h>
#include "linkstack.h"

struct _node_t_ {
	int data;
	struct _node_t_ *next;
};

struct _stack_t_ {
	node_t *top;
	int len;
	int max;
};

node_t *node_alloc(int data) {
	node_t *node = NULL;

	node = malloc(sizeof(node_t));
	if (node == NULL)
		return NULL;
	node->data = data;
	node->next = NULL;
	return node;
}

int node_free(node_t *node) {
	if (node == NULL)
		return -1;
	free(node);
	return 0;
}

stack_t *stack_init(int max) {
	stack_t *stack = NULL;

	stack = malloc(sizeof(stack_t));
	if (stack == NULL)
		return NULL;
	stack->top = node_alloc(0);
	if (stack->top == NULL) {
		free(stack);
		return NULL;
	}
	stack->len = 0;
	stack->max = max;
	return stack;
}

int stack_destory(stack_t *stack) {
	if (stack == NULL)
		return -1;

	node_t *node, *tmp;
	node = stack->top;
	while (node != NULL) {
		tmp = node;
		node = node->next;
		free(tmp);
	}
	free(stack);
	return 0;
}

int stack_isempty(stack_t *stack) {
	return stack->len == 0;
}

int stack_isfull(stack_t *stack) {
	return stack->len == stack->max;
}

int stack_push(stack_t *stack, int data) {
	if (stack == NULL)
		return -1;

	if (stack_isfull(stack))
		return -1;
		
	node_t *new = node_alloc(data);
	if (new == NULL)
		return -1;
	new->next = stack->top->next;
	stack->top->next = new;
	stack->len++;
	return 0;
}

int stack_pop(stack_t *stack, int *data) {
	if (stack == NULL)
		return -1;

	if (stack_isempty(stack))
		return -1;

	node_t *tmp = stack->top->next;
	stack->top->next = stack->top->next->next;
	stack->len--;
	*data = tmp->data;
	free(tmp);
	return 0;
}

int stack_foreach(stack_t *stack, datavisit_t visit) {
	if (stack == NULL)
		return -1;

	node_t *tmp = stack->top->next;
	while(tmp != NULL) {
		visit(tmp->data);
		tmp = tmp->next;
	}
	return 0;
}
