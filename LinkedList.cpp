#include "pch.h"
#include "LinkedList.h"
#include "Particle.h"

namespace LinkedList
{
	//Returns new front of tree so should be called as
	// head = PushFront(head, data);
	LinkedListNode* PushFront(LinkedListNode* head, Particle* data)
	{
		//Create ndoe
		LinkedListNode* node = new LinkedListNode();

		//Set data
		node->data = data;
		node->next = head;

		//Set the value of the tree head to this node.
		return node;
	}

	//Returns new front of tree so should be called as
	// head = DeleteFront(head, data);
	LinkedListNode* DeleteFront(LinkedListNode* head)
	{
		if (head->next == nullptr)
		{
			head->data = nullptr;
			return nullptr;
		}
		else
		{
			//Keep track of old head node.
			LinkedListNode* old_head = head;
			LinkedListNode* new_head_addr = old_head->next;

			//Delete old head data
			old_head->next = nullptr;
			old_head->data = nullptr;
			delete old_head;
			old_head = nullptr;

			//Set head node for tree
			return new_head_addr;
		}		
	}
}