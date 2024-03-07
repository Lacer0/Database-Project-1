#pragma once
#include "RecordStructs.h"
#include "BPTreeNode.h"
#include <tuple>

class BPTree {
	BPTreeNode* root;
	int MAX;
	void insertInternal(int, BPTreeNode *, BPTreeNode *);
	BPTreeNode* findParent(BPTreeNode *, BPTreeNode *);

public:
	BPTree(int MAX);
	void search(int);
	void insert(std::tuple<int, RecordAddress> recordMetaData);
	void remove(BPTreeNode* root, int key);
	void display(BPTreeNode *);
	BPTreeNode* getRoot();
	std::tuple<int, BPTreeNode*> findNodeWithKey(int key);
	BPTreeNode* findValidSibling(BPTreeNode* node);
};