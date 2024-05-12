#pragma once

typedef struct Node_s {
	struct Node* next;
	void* value;
	unsigned int length;
	unsigned int tag_name;
} Node;

typedef struct List_s {
	Node* head;
	Node* tail;
	unsigned int length;
} List;

Node* createNode(void* value, unsigned int length, unsigned int tag_name);

void deleteNode(Node* node);

List* createList(unsigned int length);

void deleteList(List* list);

void addNode(List* list, Node* node);

void removeNode(List* list, unsigned int index);

Node* findNode(List* list, unsigned int tag_name);
