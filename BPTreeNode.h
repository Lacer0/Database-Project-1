#pragma once

// Code taken from https://www.programiz.com/dsa/insertion-on-a-b-plus-tree

class BPTreeNode {
	bool IS_LEAF;
	int* key, size;
	BPTreeNode** ptr;
	friend class BPTree;

public:
	BPTreeNode(int MAX);
};