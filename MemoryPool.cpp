#include "MemoryPool.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>

MemoryPool::MemoryPool() {}

MemoryPool::MemoryPool(unsigned int diskCapacity, int blockSize) :totalMemory(diskCapacity), blockSize(blockSize) {
	// Guys note that each char is a byte
	memory = new char[diskCapacity];

	uint32_t tmp = 0;
	uintptr_t baseAddress = reinterpret_cast<uintptr_t>(memory);
	// Divide disk capacity into blocks!
	while (tmp < diskCapacity) {
		freeBlocks.push_back(baseAddress + tmp);
		tmp += blockSize;
	}
}

uintptr_t MemoryPool::allocateBlock() {
	if (!freeBlocks.empty()) {
		uintptr_t tmp = freeBlocks.back();
		freeBlocks.pop_back();
		return tmp;
	}
	std::cout << "ERROR! NO MORE FREE CHUNKS! LIKELY TO FAIL!" << std::endl;
}

void MemoryPool::deallocateBlock(uintptr_t address) {
	freeBlocks.push_back(address);
}

RecordAddress MemoryPool::writeRecord(Record record) {
	// If no free memory location
	if (freeRecords.empty()) {
		uintptr_t blockAddress = allocateBlock();
		uint32_t offset = 0;
		while (offset < blockSize) {
			freeRecords.push_back(RecordAddress{ blockAddress, offset });
			offset += sizeof(Record);
		}
	}

	// Get free memory location
	RecordAddress target = freeRecords.back();
	freeRecords.pop_back();

	// Write to memory location
	void* memLoc = (void*)(target.blockAddress + target.offset);
	memcpy(memLoc, &record, sizeof(Record));

	// Return record block offset address
	return target;
}

Record MemoryPool::readRecord(RecordAddress address) {
	// Source for thread sleeping:
	// https://stackoverflow.com/questions/4184468/sleep-for-milliseconds 
	std::this_thread::sleep_for(std::chrono::nanoseconds(1));

	// Directly calculate the address of the record within the block
	Record* record = reinterpret_cast<Record*>(reinterpret_cast<char*>(address.blockAddress) + address.offset);
	accessedDataBlock.emplace_back(address.blockAddress);

	// Return the record found at that location
	return *record;
}

void MemoryPool::deleteRecord(RecordAddress address) {
	freeRecords.push_back(address);
}

// Getters
int MemoryPool::getNumTotalBlocks() {
	return std::floor(totalMemory / blockSize);
}

int MemoryPool::getNumFreeBlocks() {
	return freeBlocks.size();
}

int MemoryPool::getDiskSize() {
	return totalMemory;
}

int MemoryPool::getBlockSize() {
	return blockSize;
}

int MemoryPool::getDataBlockAccessCount(bool onlyUniqueAccess) {
	if (onlyUniqueAccess) {
		std::vector<uintptr_t>::iterator it;
		it = std::unique(accessedDataBlock.begin(), accessedDataBlock.end());
		accessedDataBlock.resize(std::distance(accessedDataBlock.begin(), it));
	}
	
	return accessedDataBlock.size();
}
void MemoryPool::resetDataBlockAccessCount() {
	accessedDataBlock.clear();
}