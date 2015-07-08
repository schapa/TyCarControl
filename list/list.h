/*
 * list.h
 *
 *  Created on: Jul 7, 2015
 *      Author: shapa
 */

#ifndef LIST_H_
#define LIST_H_

#include <stddef.h>

template <typename T>
class List
{
private:
	struct Node {
		T data;
		Node *next;
		Node(T d, Node *n = 0):data(d), next(n) {}
	};
	Node *head;
	Node *tail;
	int count;
public:
	List(Node *h = 0):head(h), tail(h),count(0){}
	~List();
	void add(T d);
	T get();
	int length();
	bool isEmpty();
};

// destructor
template <typename T>
List<T>::~List() {
	Node *tmp;
	while(head) {
		tmp = head;
		head = head->next;
		delete tmp;
	}
}

template <typename T>
void List<T>::add(T d) {
	Node *new_node = new Node(d,0);
	count++;
	if(!head) {
		head = new_node;
		tail = head;
		return;
	}
	if(!tail->next) {
		tail = tail->next = new_node;
		return;
	}
}

template <typename T>
T List<T>::get() {
	if(!head)
		return NULL;
	Node *tmp = head;
	T data (head->data);
	count--;
	if(head->next) {
		head = head->next;
		delete tmp;
		return data;
	}
	delete tmp;
	head = NULL;
	return data;
}

template <typename T>
int List<T>::length() {
	return count;
}

template <typename T>
bool List<T>::isEmpty() {
	return count == 0;
}

#endif /* LIST_H_ */
