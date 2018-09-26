/* 
    File: my_allocator.h

    Original Author: R.Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/08

    Modified:

 */

#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

struct BlockHeader{
	int blockSize;
	bool blockFull;
	BlockHeader* next;
		
	BlockHeader(int blockSize, bool blockFull = false, BlockHeader* next = NULL){
		blockSize = blockSize;
		blockFull = blockFull;
		next = next;
	}
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeader s. 
private:
	BlockHeader* head;		// you need a head of the list
public:
	void insert (BlockHeader* b){			// adds a block to the list
			b->blockFull = false;
			cout << "[insert] Inserting block b: " << b << endl;
			cout << "[insert] size: " << b->blockSize << endl;
			cout << "[insert] isFull: " << b->blockFull << endl;
			b->blockFull = false;
			if (head == NULL){
				head = b;
				b->next = NULL;
			}
			else{
				b->next = head;
				head = b;
			}
			// cout << "[insert] next: " << b->next << endl;
				

		}

		void remove (BlockHeader* b){  // removes a block from the list
			cout << "[remove] Looking for b: " << b << endl;
			cout << "[remove] size: " << b->blockSize << endl;
			cout << "[remove] isFull: " << b->blockFull << endl;
			cout << "[remove] next: " << b->next << endl;
			BlockHeader* curr = head;
			while (curr != NULL){
				if (curr == b){ //Block is in the front
					head = b->next;
					b->next = NULL;
					return;
				}
				else if(curr->next == b){
					curr->next = b->next;
					b->next = NULL;
					return;
				}
				curr = curr->next;
			}
			
		}
		
		BlockHeader* getHead(){
			return head;
		}
		
		BlockHeader* findFreeBlockLL(){
			BlockHeader* curr = head;
			// cout << "[findFreeBlockLL] Searching LinkedList..." << endl;
			
			while (curr != NULL){
				if (curr->blockFull == false){
					return curr;
				}
				else{
					curr = curr->next;
				}
			}
			return NULL;
		}
		
		int getSize(){
			BlockHeader* curr = head;
			int count = 0;
			while (curr != NULL){
				count += 1;
				curr = curr->next;
				cout << curr << " ";
			}
			
			return count;
		}
};


class BuddyAllocator{
private:
	
	char* start;
	int bb; //basic block size
	int tm; //total memory
	
	vector<LinkedList> freeVec;
	

private:
	
	BlockHeader* getbuddy (BlockHeader * addr); 
	// given a block address, this function returns the address of its buddy 
	
	bool arebuddies (BlockHeader* block1, BlockHeader* block2);
	// checks whether the two blocks are buddies are not

	char* merge (char* block1, char* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	BlockHeader* split (BlockHeader* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected
	
	bool testInputs(int bb, int tm); //Helper to constructor
	
	int vecSize(int _basic_block_size, int _total_memory_length); //calculates size of freeVec
	
	int getVecIndex(int allocSize); 
	
	int powerTwoCeil(int _length);
	
	BlockHeader* findFreeBlock(int length);
	
	BlockHeader* splitToSize(BlockHeader* freeBlock, int length);
	
	bool buddyMergeAvailable(char* b);
	
	char* getbuddy(char* addr);


public:
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	

	~BuddyAllocator(); 

	char* alloc(int _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	int free(char* _a); 
	/* Frees the section of physical memory previously allocated. 
	Returns 0 if everything ok. */ 
   
	void debug ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point.*/
};

#endif 
