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

			for (int j = cursor->size; j > i; j--) {
				cursor->key[j] = cursor->key[j - 1];
				cursor->adrs[j] = cursor->adrs[j - 1];
			}
			cursor->key[i] = x;
			cursor->adrs[i] = r;
			cursor->size++;
			cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
			cursor->ptr[cursor->size - 1] = nullptr;
		}
		// Else restructure
		else {
			BPTreeNode* newLeaf = new BPTreeNode(MAX);
			int* virtualNode = new int[MAX + 1];
			RecordAddress* tempAddresses = new RecordAddress[MAX + 1];
			for (int i = 0; i < MAX; i++) {
				virtualNode[i] = cursor->key[i];
				tempAddresses[i] = cursor->adrs[i];
			}
			int i = 0, j;
			while (x > virtualNode[i] && i < MAX) {
				i++;
			}
			for (int j = MAX; j > i; j--) {
				virtualNode[j] = virtualNode[j - 1];
				tempAddresses[j] = tempAddresses[j - 1];
			}
			virtualNode[i] = x;
			tempAddresses[i] = r;
			newLeaf->IS_LEAF = true;
			cursor->size = ceil((MAX + 1) / 2);
			newLeaf->size = floor((MAX + 1) / 2);
			cursor->ptr[cursor->size] = newLeaf;
			newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
			cursor->ptr[MAX] = nullptr;
			for (i = 0; i < cursor->size; i++) {
				cursor->key[i] = virtualNode[i];
				cursor->adrs[i] = tempAddresses[i];
			}
			for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
				newLeaf->key[i] = virtualNode[j];
				newLeaf->adrs[i] = tempAddresses[j];
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
		cursor->size = (MAX + 1) / 2;
		newInternal->size = MAX - (MAX + 1) / 2;
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

void BPTree::display(BPTreeNode* cursor) {
	if (cursor != NULL) {
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
			std::cout << cursor->key[i] << " ";
			//std::cout << cursor->adrs[i].blockAddress << " ";
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

std::tuple<int, BPTreeNode*> BPTree::findNodeWithKey(int key) {
	if (root == nullptr) {
		root = new BPTreeNode(MAX);
		root->key[0] = key;
		root->IS_LEAF = true;
		root->size = 1;
	}
	else {
		BPTreeNode* cursor = root;
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
		int i;
		for (i = 0; i < cursor->size; i++) {
			if (cursor->key[i] == key) {
				std::cout << "Found key " << key << "!\n";
				return std::make_tuple(i, cursor);
			}
		}
		std::cout << "Key not found\n";
	}
	return std::make_tuple(-1, nullptr);
}

void BPTree::remove(BPTreeNode* root, int key) {
	auto result = findNodeWithKey(key);
	int position = std::get<0>(result);
	BPTreeNode* target = std::get<1>(result);

	while (position != -1 || target != nullptr) {
		// Case 1
		// Safe to just delete
		if (target->size - 1 > floor((MAX + 1) / 2)) {
			bool updateParent = false;
			int newParentKey;
			if (position == 0) {
				if (target->key[position] == target->key[position + 1]) {
					updateParent = false;
				}
				else {
					updateParent = true;
					newParentKey = target->key[position + 1];
				}
			}

			for (int i = position; i < MAX - 2; i++) {
				target->key[i] = target->key[i + 1];
				target->adrs[i] = target->adrs[i + 1];
				target->ptr[i] = target->ptr[i + 1];
			}

			// Reduce node key size
			target->size = target->size-1;

			// Just change parent's key to next value
			if (updateParent) {
				auto parent = findParent(root, target);
				for (int p = 0; p < parent->size; p++) {
					if(parent->key[p] == key) {
						parent->key[p] = newParentKey;
						break;
					}
				}
			}

			// Done and stop
			return;
		}
		// Case 2


		// Case 3

		// Load next
		result = findNodeWithKey(key);
		position = std::get<0>(result);
		target = std::get<1>(result);
	}

}

BPTreeNode* BPTree::findValidSibling(BPTreeNode* node) {
	// Check left


	// Check right

	// Else
	return nullptr;
}