#include "BPTree.h"
#include <iostream>
#include <vector>

BPTree::BPTree(int MAX_) {
	MAX = MAX_;
	root = nullptr;
}

void BPTree::search(int x) {
	if (root == nullptr) {
		root = new BPTreeNode(MAX);
		root->key[0] = x;
		root->IS_LEAF = true;
		root->size = 1;
	}
	else {
		BPTreeNode* cursor = root;
		while (cursor->IS_LEAF == false) {
			for (int i = 0; i < cursor->size; i++) {
				if (x < cursor->key[i]) {
					cursor = cursor->ptr[i];
					break;
				}

				if (i == cursor->size - 1) {
					cursor = cursor->ptr[i + 1];
					break;
				}
			}
		}
		for (int i = 0; i < cursor->size; i++) {
			if (cursor->key[i] == x) {
				std::cout << "Found key " << x << "!\n";
				return;
			}
		}
		std::cout << "Key not found\n";
	}
}

void BPTree::insert(std::tuple<int, RecordAddress> record) {

	int x = std::get<0>(record);
	RecordAddress r = std::get<1>(record);

	if (root == nullptr) {
		root = new BPTreeNode(MAX);
		root->key[0] = x;
		root->adrs[0] = r;
		root->IS_LEAF = true;
		root->size = 1;
	}
	else {
		BPTreeNode* cursor = root;
		BPTreeNode* parent = nullptr;

		// Edge case
		// Check if there are duplicates
		if (findKey(root, x) != nullptr) {
			BPTreeNode* tmp = findKey(root, x);
			// Grab index
			int index = 0;
			for (index; index < MAX; index++) {
				if (tmp->key[index] == x) {
					break;
				}
			}
			// Chain BPTreeNodes
			// First time
			BPTreeNode* node = tmp->ptr[index];
			if (node == nullptr) {
				node = new BPTreeNode(MAX);
				tmp->ptr[index] = node;
				node->key[0] = x;
				node->adrs[0] = r;
				node->size = 1;
			}
			// If not first time
			else {
				// Step through get non-filled node
				while(node->size == MAX) {
					node = node->ptr[0];
				}
				node->key[node->size] = x;
				node->adrs[node->size] = r;
				node->size++;
				if(node->size == MAX) {
					node->ptr[0] = new BPTreeNode(MAX);
					node->ptr[0]->size = 0;
				}
			}
			return;
		}

		// Get leaf node
		while (!cursor->IS_LEAF) {
			parent = cursor;

			int i = 0;
			while (i < cursor->size) {
				// If smaller take pointer on left size of key
				if (x < cursor->key[i]) {
					break;
				}
				i++;
			}
			cursor = cursor->ptr[i];
		}

		// Check if leaf node has free space
		if (cursor->size < MAX) {
			int i = 0;
			while (i < cursor->size && x > cursor->key[i]) {
				i++;
			}

			// Save next leafnode pointer
			BPTreeNode* point = cursor->ptr[cursor->size];

			for (int j = cursor->size; j > i; j--) {
				cursor->key[j] = cursor->key[j - 1];
				cursor->adrs[j] = cursor->adrs[j - 1];
				cursor->ptr[j] = cursor->ptr[j - 1];
			}
			cursor->key[i] = x;
			cursor->adrs[i] = r;
			cursor->ptr[i] = nullptr;
			cursor->size++;

			// Restore next leafnode pointer
			cursor->ptr[cursor->size] = point;
			/*cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
			cursor->ptr[cursor->size - 1] = nullptr;*/
		}
		// Else restructure
		else {
			BPTreeNode* newLeaf = new BPTreeNode(MAX);
			int* virtualNode = new int[MAX + 1];
			BPTreeNode** virtualPtr = new BPTreeNode*[MAX + 1];
			RecordAddress* tempAddresses = new RecordAddress[MAX + 1];
			for (int i = 0; i < MAX; i++) {
				virtualNode[i] = cursor->key[i];
				tempAddresses[i] = cursor->adrs[i];
				virtualPtr[i] = cursor->ptr[i];
			}
			int i = 0, j;
			while (x > virtualNode[i] && i < MAX) {
				i++;
			}
			for (int j = MAX; j > i; j--) {
				virtualNode[j] = virtualNode[j - 1];
				tempAddresses[j] = tempAddresses[j - 1];
				virtualPtr[j] = virtualPtr[j - 1];
			}
			virtualNode[i] = x;
			tempAddresses[i] = r;
			virtualPtr[i] = nullptr;
			newLeaf->IS_LEAF = true;
			cursor->size = ceil((MAX + 1) / 2.0);
			newLeaf->size = floor((MAX + 1) / 2.0);
			cursor->ptr[cursor->size] = newLeaf;
			newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
			cursor->ptr[MAX] = nullptr;
			for (i = 0; i < cursor->size; i++) {
				cursor->key[i] = virtualNode[i];
				cursor->adrs[i] = tempAddresses[i];
				cursor->ptr[i] = virtualPtr[i];
			}
			for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
				newLeaf->key[i] = virtualNode[j];
				newLeaf->adrs[i] = tempAddresses[j];
				newLeaf->ptr[i] = virtualPtr[j];
			}
			if (cursor == root) {
				BPTreeNode* newRoot = new BPTreeNode(MAX);
				newRoot->key[0] = newLeaf->key[0];
				newRoot->adrs[0] = newLeaf->adrs[0];
				newRoot->ptr[0] = cursor;
				newRoot->ptr[1] = newLeaf;
				newRoot->IS_LEAF = false;
				newRoot->size = 1;
				root = newRoot;
			}
			else {
				insertInternal(newLeaf->key[0], parent, newLeaf);
			}
			delete[] virtualNode;
			delete[] tempAddresses;
			delete[] virtualPtr;
		}
	}
}

void BPTree::insertInternal(int x, BPTreeNode* cursor, BPTreeNode* child) {
	if (cursor->size < MAX) {
		int i = 0;
		while (x > cursor->key[i] && i < cursor->size) {
			i++;
		}
		for (int j = cursor->size; j > i; j--) {
			cursor->key[j] = cursor->key[j - 1];
		}
		for (int j = cursor->size + 1; j > i + 1; j--) {
			cursor->ptr[j] = cursor->ptr[j - 1];
		}
		cursor->key[i] = x;
		cursor->size++;
		cursor->ptr[i + 1] = child;
	}
	else {
		BPTreeNode* newInternal = new BPTreeNode(MAX);
		int* virtualKey = new int[MAX + 1];
		BPTreeNode** virtualPtr = new BPTreeNode * [MAX + 2];
		for (int i = 0; i < MAX; i++) {
			virtualKey[i] = cursor->key[i];
		}
		for (int i = 0; i < MAX + 1; i++) {
			virtualPtr[i] = cursor->ptr[i];
		}
		int i = 0, j;
		while (x > virtualKey[i] && i < MAX) {
			i++;
		}
		for (int j = MAX; j > i; j--) {
			virtualKey[j] = virtualKey[j - 1];
		}
		virtualKey[i] = x;
		for (int j = MAX + 1; j > i + 1; j--) {
			virtualPtr[j] = virtualPtr[j - 1];
		}
		virtualPtr[i + 1] = child;
		newInternal->IS_LEAF = false;
		cursor->size = ceil(MAX / 2.0);
		newInternal->size = floor(MAX / 2.0);
		for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++) {
			newInternal->key[i] = virtualKey[j];
		}
		for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++) {
			newInternal->ptr[i] = virtualPtr[j];
		}

		if (cursor == root) {
			BPTreeNode* newRoot = new BPTreeNode(MAX);
			newRoot->key[0] = cursor->key[cursor->size];
			newRoot->ptr[0] = cursor;
			newRoot->ptr[1] = newInternal;
			newRoot->IS_LEAF = false;
			newRoot->size = 1;
			root = newRoot;
		}
		else {
			insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
		}
		delete[] virtualKey;
		delete[] virtualPtr;
	}
}

BPTreeNode* BPTree::findParent(BPTreeNode* cursor, BPTreeNode* child) {
	BPTreeNode* parent = nullptr;
	if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF) {
		return nullptr;
	}
	for (int i = 0; i < cursor->size + 1; i++) {
		if (cursor->ptr[i] == child) {
			parent = cursor;
			return parent;
		}
		else {
			parent = findParent(cursor->ptr[i], child);
			if (parent != nullptr)
				return parent;
		}
	}
	return parent;
}

BPTreeNode* BPTree::findKey(BPTreeNode* cursor, int key) {
	if (cursor != nullptr) {
		while (cursor->IS_LEAF == false) {
			for (int i = 0; i < cursor->size; i++) {
				if (key < cursor->key[i]) {
					cursor = cursor->ptr[i];
					break;
				}

				if (i == cursor->size - 1) {
					cursor = cursor->ptr[i + 1];
					break;
				}
			}
		}
		for (int i = 0; i < cursor->size; i++) {
			if (cursor->key[i] == key) {
				return cursor;
			}
		}
	}
	return nullptr;
}

void BPTree::deleteAllWithKeyValue(int key) {
	int deleteCount = 0;
	while (findKey(root, key) != nullptr) {
		deleteKey(key);
		deleteCount++;
	}
	std::cout << "Delete count: " << deleteCount << std::endl;
}

void BPTree::deleteKey(int x) {
	if (root == nullptr) {
		std::cout << "Tree is empty\n";
		return;
	}

	// Step 1: Find leaf node containing key
	BPTreeNode* cursor = root;
	BPTreeNode* parent = nullptr;
	while (!cursor->IS_LEAF) {
		parent = cursor;
		int i = 0;
		while (i < cursor->size && x >= cursor->key[i]) {
			i++;
		}
		cursor = cursor->ptr[i];
	}

	// Step 2: Delete the key from the leaf node
	bool found = false;
	int i;
	for (i = 0; i < cursor->size; i++) {
		if (cursor->key[i] == x) {
			found = true;
			break;
		}
	}

	if (!found) {
		std::cout << "Key not found\n";
		return;
	}

	for (; i < cursor->size - 1; i++) {
		cursor->key[i] = cursor->key[i + 1];
		cursor->adrs[i] = cursor->adrs[i + 1];
		cursor->ptr[i] = cursor->ptr[i + 1];
	}
	cursor->size--;

	// Steps 3 and 4, check for underflow and handle it:
	if (cursor->size < ceil((MAX / 2.0) - 1)) {
		handleUnderflow(cursor, parent);
	}
}

void BPTree::handleUnderflow(BPTreeNode* cursor, BPTreeNode* parent) {
	// Find sibling
	int siblingIndex = 0;
	for (; siblingIndex <= parent->size; siblingIndex++) {
		if (parent->ptr[siblingIndex] == cursor) {
			break;
		}
	}

	BPTreeNode* leftSibling = nullptr;
	BPTreeNode* rightSibling = nullptr;

	if (siblingIndex > 0) {
		leftSibling = parent->ptr[siblingIndex - 1];
	}

	if (siblingIndex < parent->size) {
		rightSibling = parent->ptr[siblingIndex + 1];
	}

	// Try redistributing first
	if (leftSibling != nullptr && leftSibling->size > ceil((MAX / 2.0) - 1)) {
		// Redistribute with left sibling
		redistribute(cursor, leftSibling, parent, siblingIndex - 1, true);
	}
	else if (rightSibling != nullptr && rightSibling->size > ceil((MAX / 2.0) - 1)) {
		// Redistribute with right sibling
		redistribute(cursor, rightSibling, parent, siblingIndex, false);
	}
	else {
		// Merge with sibling
		if (leftSibling != nullptr) {
			mergeNodes(cursor, leftSibling, parent, siblingIndex);
		}
		else if (rightSibling != nullptr) {
			mergeNodes(cursor, rightSibling, parent, siblingIndex + 1);
		}
	}
}

void BPTree::redistribute(BPTreeNode* cursor, BPTreeNode* sibling, BPTreeNode* parent, int parentIndex, bool isLeftSibling) {
	if (isLeftSibling) {
		// Move a key from the left sibling to underflow node

		for (int i = cursor->size; i > 0; i--) {
			cursor->key[i] = cursor->key[i - 1];
			cursor->adrs[i] = cursor->adrs[i - 1];
			cursor->ptr[i] = cursor->ptr[i - 1];
		}
		cursor->key[0] = sibling->key[sibling->size - 1];
		cursor->adrs[0] = sibling->adrs[sibling->size - 1];
		cursor->ptr[0] = sibling->ptr[sibling->size - 1];
		cursor->size++;

		sibling->size--;

		// Update parent key
		parent->key[parentIndex] = cursor->key[0];
	}
	else {
		// Move one key from the right sibling to the underflow node
		cursor->key[cursor->size] = sibling->key[0];
		cursor->adrs[cursor->size] = sibling->adrs[0];
		cursor->ptr[cursor->size] = sibling->ptr[0];
		cursor->size++;
		for (int i = 0; i < sibling->size - 1; i++) {
			sibling->key[i] = sibling->key[i + 1];
			sibling->adrs[i] = sibling->adrs[i + 1];
			sibling->ptr[i] = sibling->ptr[i + 1];
		}
		sibling->size--;

		// Update parent key
		parent->key[parentIndex] = sibling->key[0];
	}
}

void BPTree::mergeNodes(BPTreeNode* cursor, BPTreeNode* sibling, BPTreeNode* parent, int mergeIndex) {
	if (cursor->size + sibling->size < MAX) {
		// Merge cursor into sibling
		for (int i = 0; i < cursor->size; i++) {
			sibling->key[sibling->size + i] = cursor->key[i];
			sibling->adrs[sibling->size + i] = cursor->adrs[i];
			sibling->ptr[sibling->size + i] = cursor->ptr[i];
		}
		sibling->size += cursor->size;
		sibling->ptr[sibling->size] = cursor->ptr[cursor->size];

		// Remove cursor's entry from parent
		for (int i = mergeIndex; i < parent->size - 1; i++) {
			parent->key[i] = parent->key[i + 1];
			parent->ptr[i] = parent->ptr[i + 1];
		}
		parent->size--;

		// Delete cursor
		delete[] cursor->key;
		delete[] cursor->ptr;
		delete cursor;

		// Check underflow in parent
		if (parent != root && parent->size < ceil((MAX / 2.0) - 1)) {
			BPTreeNode* grandParent = findParent(root, parent);
			handleUnderflow(parent, grandParent);
		}
	}
}

void BPTree::display(BPTreeNode* cursor) {
	if (cursor != nullptr) {
		if (cursor == root) {
			std::cout << "Root Node: ";
		}
		else if (cursor != root && !cursor->IS_LEAF) {
			std::cout << "Internal Node: ";
		}
		else {
			std::cout << "Leaf Node: ";
		}
		for (int i = 0; i < cursor->size; i++) {
			std::cout << cursor->key[i];

			// print duplicates
			if (cursor->IS_LEAF && cursor->ptr[i] != nullptr) {
					int countDups = 0;
					BPTreeNode* tmp = cursor->ptr[i];
					while (tmp != nullptr) {
						countDups += tmp->size;
						//for (int i = 0; i < tmp->size; i++) {
						//	std::cout << tmp->key[i] << " ";
						//}
						tmp = tmp->ptr[0];
					}
					std::cout << "(" << countDups << ")";
				}
			//std::cout << cursor->adrs[i].blockAddress << " ";
			std::cout << " ";
		}
		std::cout << "\n";
		if (cursor->IS_LEAF != true) {
			for (int i = 0; i < cursor->size + 1; i++) {
				display(cursor->ptr[i]);
			}
		}
	}
}

// Get the root
BPTreeNode* BPTree::getRoot() {
	return root;
}

int BPTree::countNodes() const {
	return countNodesHelper(root);
}

int BPTree::countNodesHelper(BPTreeNode* node) const {
	if (node == nullptr) return 0; // Base case

	int count = 1; // Count the current node
	if (!node->IS_LEAF) {
		// Iterate through all child pointers
		for (int i = 0; i <= node->size; ++i) {
			count += countNodesHelper(node->ptr[i]);
		}
	}
	return count;
}

int BPTree::treeHeight() const {
	int height = 0;
	BPTreeNode* currentNode = root;
	while (currentNode != nullptr && !currentNode->IS_LEAF) {
		currentNode = currentNode->ptr[0]; // Always follow the first child pointer
		height++;
	}
	return height + 1; // Add 1 to count the leaf level
}

void BPTree::printRootKeys() const {
	if (root == nullptr) {
		std::cout << "The tree is empty." << std::endl;
		return;
	}

	std::cout << "Keys in the root node: ";
	for (int i = 0; i < root->size; ++i) {
		std::cout << root->key[i] << " ";
	}
	std::cout << std::endl;
}


void BPTree::fetchRecordsByRange(BPTreeNode* node, int min, int max, int& numIndexAccessed, MemoryPool& memPool, std::vector<Record>& records) {
	int numDataAccessed = 0;
	if (node == nullptr) {
		return;
	}

	while (node->IS_LEAF == false) {
		for (int i = 0; i < node->size; i++) {
			numIndexAccessed++;
			if (min < node->key[i]) {
				node = node->ptr[i];
				break;
			}
			if (i == node->size - 1) {
				node = node->ptr[i + 1];
				break;
			}
		}
	}
	bool stop = false;
	while (node != nullptr) {
		for (int i = 0; i < node->size; i++) {
			if (node->key[i] >= min && node->key[i] <= max) {
				records.push_back(memPool.readRecord(node->adrs[i]));
				numDataAccessed++;

				// Check for duplicates
				BPTreeNode* temp = node->ptr[i];
				while (temp != nullptr) {
					numIndexAccessed++;
					for (int dup = 0; dup < temp->size; dup++) {
						records.push_back(memPool.readRecord(temp->adrs[dup]));
						numDataAccessed++;
					}
					// Go down the chain
					temp = temp->ptr[0];
				}
			}
			else if (node->key[i] > max) {
				// stop early
				// stop = true;
				break;
			}
		}
		if (stop) {
			break;
		}
		// Go next leaf node
		if (node->ptr[node->size] != nullptr) {
			numIndexAccessed++;
			node = node->ptr[node->size];
		}
		else {
			break;
		}
	}
}