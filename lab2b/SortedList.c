#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "SortedList.h"


void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
	if (list == NULL) return;

	SortedListElement_t *curr = list;
	SortedListElement_t *next = curr->next;

	while (next != NULL){
		if (opt_yield & INSERT_YIELD) sched_yield();

		if (strcmp(element->key, next->key) <= 0){
			curr->next = element;
			element->prev = curr;
			element->next = next;
			next->prev = element;
			return;
		}
		curr = next;
		next = next->next;
	}

	//If we get to the end and havent inserted tack it on at the end
	curr->next = element;
	element->prev = curr;
	element->next = NULL;
}

int SortedList_delete(SortedListElement_t *element){
	if (element == NULL) return 0;

	SortedListElement_t *prev = element->prev;
	SortedListElement_t *next = element->next;

	if (opt_yield & DELETE_YIELD) sched_yield();

	if (prev != NULL) {
		if (prev->next != element) {
			return 1;
		}
		prev->next = next;
	}
	if (next != NULL) {
		if (next->prev != element) {
			return 1;
		}
		next->prev = prev;
	}
	element->prev = NULL;
	element->next = NULL;

	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
	if (list == NULL) return NULL;

	//Start looking after the head
	SortedListElement_t *curr = list->next;
	SortedListElement_t *next;

	while (curr != NULL){
		if (opt_yield & LOOKUP_YIELD) sched_yield();

		next = curr->next;
		if (strcmp(curr->key,key) == 0) return curr;
		curr = next;
	}
	return NULL;
}

int SortedList_length(SortedList_t *list){
	if (list == NULL) return 0;

	SortedListElement_t *prev = list;
	//Start counting after the head
	SortedListElement_t *curr = list->next;
	SortedListElement_t *next;

	int length = 0;
	while (curr != NULL){
		if (opt_yield & LOOKUP_YIELD) sched_yield();

		length++;
		next = curr->next;
		//list sanity check
		if (prev != NULL && prev->next != curr) {
			return -1;
		}
		if (next != NULL && next->prev != curr) {
			return -1;
		}

		prev = curr;
		curr = next;
	}
	return length;
}

