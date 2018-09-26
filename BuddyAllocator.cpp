/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
	cout << "*****Initializing Buddy Allocator*****" << endl;
	
	//Test Inputs
	while (testInputs(_basic_block_size, _total_memory_length) == false){
		cin >> _basic_block_size;
		cin >> _total_memory_length;
	}
	
	//Initialize Allocator Members
	bb = _basic_block_size;
	tm = _total_memory_length;
	
	start = new char[_total_memory_length]; //System call
	freeVec.resize(vecSize(bb,tm)); // Set size of Vector
	for (int i = 0; i < freeVec.size(); i++){
		freeVec.at(i) = LinkedList();
	}
	
	BlockHeader* firstHeader = (BlockHeader*) start;
	firstHeader->blockSize = tm;
	firstHeader->blockFull = false;
	firstHeader->next = NULL;
	freeVec.at(getVecIndex(firstHeader->blockSize)).insert(firstHeader);
	cout << freeVec.at(vecSize(bb, tm) - 1).getHead()->blockSize << endl;
	debug();
}

bool BuddyAllocator::testInputs(int bb, int tm){
	if (bb > tm){
		return false;
	}
	if (powerTwoCeil(bb) != log2(bb)){
		return false;
	}
	if (powerTwoCeil(tm) != log2(tm)){
		return false;
	}
	return true;
}


int BuddyAllocator::vecSize(int _basic_block_size, int _total_memory_length){
	return log2(_total_memory_length) - log2(_basic_block_size) + 1;
}

int BuddyAllocator::getVecIndex(int allocSize){
	return powerTwoCeil(allocSize) - log2(bb);
}

int BuddyAllocator::powerTwoCeil(int _length){
	return ceil(log2(_length));
}








char* BuddyAllocator::alloc(int length) {
	
	cout << "*****Allocating " << length << " bytes*****" << endl;
	
	length += sizeof(BlockHeader);
	
	if (length > tm){
		return NULL;
	}
	
	if (length < bb){
		length = bb;
	}
	else{
		length = pow(2,powerTwoCeil(length));
	}

	
	BlockHeader* freeBlock = findFreeBlock(length);
	
	if (freeBlock == NULL){
		return NULL;
	}
	
	freeBlock = splitToSize(freeBlock, length);
	
	freeVec.at(getVecIndex(length)).remove(freeBlock);
	
	freeBlock->blockFull = true;
	
	cout << "[alloc] freeBlock address: " << freeBlock << endl;
	cout << "[alloc] freeBlock size: " << freeBlock->blockSize << endl;
	cout << "[alloc] freeBlock isFull: " << freeBlock->blockFull << endl;
	cout << "[alloc] freeBlock next: " << freeBlock->next << endl;
	
	debug();
	
	return (char*) freeBlock + sizeof(BlockHeader);
	
}

BlockHeader* BuddyAllocator::findFreeBlock(int length){
	
	int index = getVecIndex(length);
	BlockHeader* freeBlock = NULL;
	
	while (index <= getVecIndex(tm)){
		freeBlock = freeVec.at(index).findFreeBlockLL();
		
		if (freeBlock != NULL){
			return freeBlock;
		}
		
		index += 1;
	}
	
	return freeBlock;
}

BlockHeader* BuddyAllocator::splitToSize(BlockHeader* freeBlock, int length){
	
	while(freeBlock->blockSize > length){
		freeBlock = split(freeBlock);
	}
	
	return freeBlock;
}

BlockHeader* BuddyAllocator::split(BlockHeader* block){
	
	int startSize = block->blockSize;
	BlockHeader* newPtr = (BlockHeader*) ((char*) block + (startSize / 2));
	
	
	block->blockSize = startSize/2;
	block->blockFull = false;
	
	newPtr->blockSize = startSize/2;
	newPtr->next = NULL;
	newPtr->blockFull = false;
	
	int vecPlace = getVecIndex(startSize/2);
	
	freeVec.at(vecPlace + 1).remove(block);
	block->next = NULL;
	
	freeVec.at(vecPlace).insert(newPtr);
	
	freeVec.at(vecPlace).insert(block);
	
	// debug();
	
	return block;
}
	
	
	

int BuddyAllocator::free(char* _a) {
  _a = _a - sizeof(BlockHeader);
	cout << "*****User Freeing Block*****" << endl;
	
	if ( ((BlockHeader*) _a)->blockFull == false){
		// cout << "block is already empty" << endl;
		return 0;
	}
	
	while (((BlockHeader*) _a)->blockSize < tm && buddyMergeAvailable(_a)){
		_a = merge(_a, getbuddy(_a));
	}
	
	((BlockHeader*) _a)->blockFull = false;
	
	cout << "[free] Block Address: " << (BlockHeader*)_a << endl;
	cout << "[free] BlockSize: " <<  ((BlockHeader*)_a)->blockSize << endl;
	cout << "[free] Block isFull: " << ((BlockHeader*)_a)->blockFull << endl;
	cout << "[free] Block next: " << ((BlockHeader*)_a)->next << endl;
	
	debug();
	
  return 0;
}

bool BuddyAllocator::buddyMergeAvailable(char* b){
	
	
	char* budAddress = getbuddy(b);
	if (!arebuddies((BlockHeader*)b, (BlockHeader*) budAddress)){
		return false;
	}
	
	if (((BlockHeader*) budAddress)->blockFull == true){
		freeVec.at(getVecIndex(((BlockHeader*) b)->blockSize)).remove((BlockHeader*) b);
		freeVec.at(getVecIndex(((BlockHeader*) b)->blockSize)).insert((BlockHeader*) b);
		return false;
	}
	
	return true;
}

bool BuddyAllocator::arebuddies(BlockHeader* block1, BlockHeader* block2){
	
	if (((BlockHeader*)getbuddy( (char*) block1)) != block2){
		return false;
	}
	if (((BlockHeader*) block1)->blockSize != ((BlockHeader*) block2)->blockSize){
		return false;
	}
	
	return true;
}

char* BuddyAllocator::getbuddy(char* addr){
	
    BlockHeader* curr = (BlockHeader*) addr;
    uint blockSize = curr->blockSize;
    unsigned long offset = (unsigned long)addr - (unsigned long)start;
    return (char*) ((offset ^ (blockSize)) + (unsigned long)start);
}

char* BuddyAllocator::merge(char* block1, char* block2){
	
	
	
	BlockHeader* rb1;
	BlockHeader* rb2;
	
	if (((BlockHeader*) block1) < (BlockHeader*) block2){
		rb1 = (BlockHeader*) block1;
		rb2 = (BlockHeader*) block2;
	}else{
		rb1 = (BlockHeader*) block2;
		rb2 = (BlockHeader*) block1;
	}
	
	rb1->blockSize = rb1->blockSize + rb2->blockSize;
	
	freeVec.at(getVecIndex(rb2->blockSize)).remove(rb2);
	
	freeVec.at(getVecIndex(rb2->blockSize)).remove(rb1);
	
	freeVec.at(getVecIndex(rb1->blockSize)).insert(rb1);
	
	rb1->blockFull = false;
		
	return (char*) rb1;
	
}



void BuddyAllocator::debug (){
	cout << endl;
    for (int i = 0; i < freeVec.size(); i++){
	  cout << i << " Linked List Size: " << freeVec.at(i).getSize() << endl;
	}
	cout << endl;
}

BuddyAllocator::~BuddyAllocator (){
	delete[] start;
	
}