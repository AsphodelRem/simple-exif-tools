#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <linked_list.h>

Node* createNode(void* value, unsigned int length, char* tag_name)
{
	if (length == 0) {
		return NULL;
	}

	Node* new_node = (Node*)calloc(1, sizeof(Node));

	// Avoiding NULL pointer dereferencing
	if (new_node == NULL) {
		return NULL;
	}

	new_node->value			= (void*)calloc(length, 1);
	new_node->length		= length;
	int tag_name_length		= strlen(tag_name) + 1;
	new_node->tag_name		= (char*)calloc(tag_name_length, sizeof(char));
	new_node->next			= NULL;

	if (new_node->tag_name != NULL) {
		strncpy(new_node->tag_name, tag_name, tag_name_length);
	}

	if (value != NULL && new_node->value) {
		memcpy(new_node->value, value, length);
	}

	return new_node;
}

void deleteNode(Node* node) {
	if (node->value != NULL) {
		free(node->value);
	}

	free(node);
}

List* createList(unsigned int length) {
	List* new_list = (List*)calloc(1, sizeof(List));

	// Avoiding NULL pointer dereferencing
	if (new_list == NULL) {
		return NULL;
	}

	new_list->length = 0;
	new_list->tail = new_list->head;

	return new_list;
}

void deleteList(List* list) {
	if (list) {
		Node* iter = list->head;
		while (iter != NULL) {
			Node* next = iter->next;
			free(iter);
			iter = next;
		}

		free(list);
	}
}

void addNode(List* list, Node* node) {
	if (list && node) {

		if (list->length == 0) {
			list->head = node;
			list->tail = node;
		}

		else {
			node->next = NULL;
			list->tail->next = node;
			list->tail = node;
		}

		list->length++;
	}
}

void removeNode(List* list, unsigned int index) {
	if (index >= list->length) { return; }

	if (list->length == 1) {
		deleteNode(list->head);
	}

	if (index == 0) {
		Node* new_head = list->head->next;
		deleteNode(list->head);
		list->head = new_head;
	}

	else {
		Node* current = list->head;
		Node* previous = NULL;

		for (unsigned int i = 0; i < index; i++) {
			previous = current;
			current = current->next;
		}

		if (current != NULL) {
			if (previous != NULL) {
				previous->next = current->next;
			}
			deleteNode(current);
		}
	}

	list->length--;
}

Node* findNode(List* list, unsigned int tag_name) {
	Node* iter = list->head;
	while (iter != NULL) {
		if (iter->tag_name == tag_name) {
			return iter;
		}
		iter = iter->next;
	}

	return NULL;
}
