// Project 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include "MemoryPool.h"
#include "RecordStructs.h"
#include <fstream>
#include <regex>
#include "BPTreeNode.h"
#include "BPTree.h"

struct BlockPointer {
    int id;
    void* pointer;
};

int main()
{
    int blockSize = 200;
    int totalMemory = 524288000;
    MemoryPool memPool(totalMemory, blockSize);

    // Read data.tsv
    // Reference "https://www.w3schools.com/cpp/cpp_files.asp"
    std::ifstream data("data.tsv");

    // Use a while loop together with the getline() function to read the file line by line
    std::string line;
    bool firstLine = true;

    // https://en.cppreference.com/w/cpp/regex/regex_match
    std::regex expr = std::regex("^(tt)(\\d+)\\t([\\d*\\.?\\d*]+)\\t(\\d+)$");
    std::smatch match;

    std::queue<std::tuple<int, RecordAddress>> dataset;
    int stopAt = 0;
    while (std::getline(data, line)) {
        if (std::regex_match(line, match, expr)) {
            // Apparently regex array starts from 1 lol
            int tconst = std::stoi(match[2]); // Without tt at the start of the string
            float averageRating = std::stof(match[3]);
            int numVotes = std::stoi(match[4]);

            Record record = { tconst, averageRating, numVotes };
            RecordAddress recordAddress = memPool.writeRecord(record);

            dataset.push(std::tuple<int, RecordAddress>(numVotes, recordAddress));

            if (stopAt == 5000) {
                break;
            }
            stopAt++;
        }
    }

    std::cout << "================== Experiment 1 ==================" << std::endl;
    std::cout << "Number of records: " << dataset.size() << std::endl;
    std::cout << "Size of a record: " << sizeof(Record) << " bytes" << std::endl;
    std::cout << "Number of records stored in a block: " << std::floor(memPool.getBlockSize() / sizeof(Record)) << std::endl;
    std::cout << "Number of blocks for storing the data: " << memPool.getNumTotalBlocks() - memPool.getNumFreeBlocks() << std::endl;

    // <Block header> <pointer><key><pointer><key><pointer>
    // Need to take care of last pointer, so remove it from available blocksize
    int n = std::floor((blockSize - sizeof(BPTreeNode) - sizeof(BPTreeNode*))/sizeof(BPTreeNode*) + sizeof(int));

    std::cout << "Size of n: " << n << "\n";

    BPTree tree(n);
    
    // Insert numVotes into B+ tree
    while (!dataset.empty()) {
        tree.insert(dataset.front());
        dataset.pop();
    }

    //tree.deleteAllWithKeyValue(6);
    tree.deleteKey(6);
    tree.deleteKey(9);
    tree.deleteKey(29);
    tree.deleteKey(30);
    tree.deleteKey(31);
    tree.deleteKey(32);
    tree.deleteKey(33);
    tree.deleteKey(34);
    tree.deleteKey(35);
    tree.deleteKey(36);
    tree.deleteKey(37);
    tree.deleteKey(38);
    tree.deleteKey(39);
    
    tree.display(tree.getRoot());

}