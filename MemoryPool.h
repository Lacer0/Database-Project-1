#pragma once
#include <queue>
#include <tuple>
#include "RecordStructs.h"

class MemoryPool {
public:
	MemoryPool();
	MemoryPool(unsigned int diskCapacity, int blockSize);

	// Blocks
	uintptr_t allocateBlock();
	void deallocateBlock(uintptr_t address);

	// Record
	RecordAddress writeRecord(Record record);
	Record readRecord(RecordAddress recordAddress);
	void deleteRecord(RecordAddress address);
	//bool writeToBlock(unsigned address, char* object);

	// Getters
	int getNumTotalBlocks();
	int getNumFreeBlocks();
	int getDiskSize();
	int getBlockSize();

	int getDataBlockAccessCount();
	void resetDataBlockAccessCount();

private:
	std::vector<uintptr_t> accessedDataBlock;
	unsigned int totalMemory;
	unsigned int blockSize;
	std::vector<uintptr_t> freeBlocks;
	char* memory;
	std::vector<RecordAddress> freeRecords; // stores block, offset
};