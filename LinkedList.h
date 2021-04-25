#pragma once

class Particle;
//TODO : POSSIBLY TEMPLATE

class LinkedListNode
{
public:
	LinkedListNode* next;
	Particle* data;
};

namespace LinkedList
{
	LinkedListNode* PushFront(LinkedListNode* head, Particle* data);
	LinkedListNode* DeleteFront(LinkedListNode* head);
}