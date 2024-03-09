#pragma once
#include "RecordStructs.h"
#include "BPTreeNode.h"
#include <tuple>
#include "MemoryPool.h"

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
	BPTreeNode* findKey(BPTreeNode* cursor, int key);
	void deleteAllWithKeyValue(int key);
	void display(BPTreeNode *);
	BPTreeNode* getRoot();
	int countNodes() const;
	int treeHeight() const;
	void printRootKeys() const;
	void fetchRecordsByRange(BPTreeNode* node, int min, int max, int& numIndexAccessed, MemoryPool& memPool, std::vector<Record>& records);

private:
	int countNodesHelper(BPTreeNode* node) const;
};