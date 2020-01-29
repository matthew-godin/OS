#ifndef K_MEMORY_QUEUE_H_
#define K_MEMORY_QUEUE_H_

typedef struct node 

{
	U32 addr;
	node* next;
	
	node(U32) ();
}

typedef struct queue 
{
	node* first;
	node* last;
	void push(node*) ();
	node* pop() ();
}


#endif