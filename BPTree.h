#pragma once
#include "RecordStructs.h"
#include "BPTreeNode.h"
#include <tuple>

class BPTree {
	BPTreeNode* root;
	int MAX;
	void insertInternal(int x, BPTreeNode* cursor, BPTreeNode* child);
	BPTreeNode* findParent(BPTreeNode* cursor, BPTreeNode* child);
	void handleUnderflow(BPTreeNode* cursor, BPTreeNode* parent);
	void redistribute(BPTreeNode* cursor, BPTreeNode* sibling, BPTreeNode* parent, int parentIndex, bool isLeftSibling);
	void mergeNodes(BPTreeNode* cursor, BPTreeNode* sibling, BPTreeNode* parent, int mergeIndex);

public:
	BPTree(int MAX);
	void search(int);
	void insert(std::tuple<int, RecordAddress> recordMetaData);
	void deleteKey(int x);
	void display(BPTreeNode *);
	BPTreeNode* getRoot();

};