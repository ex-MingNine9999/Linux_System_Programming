#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ssu_backup.h"

void list_init(dList *list)		// 리스트를 초기화 하는 함수
{
	list->tail = (node *)malloc(sizeof(node));
	list->head = list->cur = list->tail;
	list->size = 0;
}

void list_pushBack(dList *list, node n)		// 리스트의 맨뒤에 노드를 추가하는 함수 
{
	node *new = (node *)malloc(sizeof(node));
	*new = n;
	list->tail->next = new;

	list->cur = list->tail;
	list->tail = list->tail->next;
	list->tail->prev = list->cur;

	list->size++;
}

node *list_search(dList *list, const char *fname)			// 리스트에서 filename이 같은 노드를 찾아 리턴하는 함수
{
	list->cur = list->head->next;

	while (list->cur != NULL) {
		if (strcmp(list->cur->filename, fname) == 0) {
			break;
		}

		list->cur = list->cur->next;
	}

	return list->cur;
}

int list_delete(dList *list, const char *fname)	// 리스트에서 filename을 찾아 삭제하는 함수
{
	node *next;

	list_search(list, fname);
	next = list->cur->next;

	if (list->cur == NULL) {
		return -1;
	}

	list->size--;
	if (list->size == 0) {
		free(list->cur);

		list_init(list);
	}
	else {
		if (list->cur != list->tail) {
			list->cur->next->prev = list->cur->prev;
		}
		else {
			list->tail = list->tail->prev;
		}
		list->cur->prev->next = list->cur->next;
		free(list->cur);
		list->cur = NULL;
	}

	list->cur = next;

	return 0;
}

void list_deleteAll(dList *list)		// 리스트의 노드들을 전부 삭제하고 초기화 하는 함수
{
	if (list->size == 0) {
		return;
	}

	list->tail = list->tail->prev;

	while(list->tail != NULL) {
		list->cur = list->tail;
		list->tail = list->tail->prev;
		free(list->cur);
		list->cur = NULL;
	}

	list_init(list);
}

void list_print(dList *list)		// 리스트를 전부 출력하는 함수 
{
	int i, j;
	const char optTable[] = OPT_TABLE;

	if (list->size == 0) {
		printf("list is empty\n");
		return;
	}

	list->cur = list->head->next;
	for (i = 0; list->cur != NULL; i++) {
		printf("%3d. FileName : %s \n     period : %d\t\toptions : ", i, list->cur->filename, list->cur->period);
		for (j = 0; j < OPT_NUM; j++) {
			if (list->cur->opt[j]) {
				printf("-%c", optTable[j]);

				if (j != OPT_D && list->cur->optarg[j][0] != 0) {
					printf(" %s", list->cur->optarg[j]);
				}
				printf("\t");
			}
		}
		
		if (list->cur->optarg[OPT_D][0] != 0) {
			printf("\n     This file is chained number %d", list->cur->optarg[OPT_D][0]);
		}
		printf("\n\n");

		list->cur = list->cur->next;
	}
}
