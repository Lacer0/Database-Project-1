#pragma once
#include "RecordStructs.h"

// Code taken from https://www.programiz.com/dsa/insertion-on-a-b-plus-tree

class BPTreeNode {
	//bool IS_LEAF;
	//int* key, size;
	//BPTreeNode** ptr;
	//friend class BPTree;

public:
	BPTreeNode(int MAX);
	bool IS_LEAF;
	int* key, size;
	BPTreeNode** ptr;
	RecordAddress* adrs;
};