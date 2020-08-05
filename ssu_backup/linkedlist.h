#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ssu_backup.h"

void list_init(dList *list)
{
	list->head = list->cur = list->tail;
	list->size = 0;
}

int list_empty(dList *list)
{
	return list->head == list->tail;
}

void list_pushBack(dList *list, node n)
{
	node *new = (node *)malloc(sizeof(node));
	*new = n;
	list->tail = new;
	if (list_empty(list)) {
		puts("1");
		list->head = list->tail;
	}

	list->tail->next = (node *)malloc(sizeof(node));
	list->tail->next->prev = list->tail;
	list->tail = list->tail->next;
	list->tail = NULL;

	list->size++;
}

int list_delete(dList *list, const char *fname)
{
	list->cur = list->head;

	while (list->cur->next != NULL) {
		printf("%s %d\n", list->cur->filename, list->cur->period);
		if (strcmp(list->cur->filename, fname) == 0) {
			break;
		}

		list->cur = list->cur->next;
	}

	if (list->cur == NULL) {
		return -1;
	}

	list->cur->next->prev = list->cur->prev;
	list->cur->prev->next = list->cur->next;
	free(list->cur);

	list->size--;

	list->cur = list->head;

	return 0;
}

void list_deleteAll(dList *list)
{
	if (list_empty(list)) {
		return;
	}

	list->tail = list->tail->prev;

	while(list->tail != NULL) {
		list->cur = list->tail;
		list->tail = list->tail->prev;
		free(list->cur);
		list->cur = NULL;
	}

	list->head = list->tail;
}
