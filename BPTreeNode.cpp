#include "BPTreeNode.h"
#include <iostream>


BPTreeNode::BPTreeNode(int MAX)
{
	key = new int[MAX];
	ptr = new BPTreeNode*[MAX + 1]();
	adrs = new RecordAddress[MAX]();
}
