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
#include <chrono>

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

    std::vector<std::tuple<int, RecordAddress>> dataset;
    int stopAt = 0;
    while (std::getline(data, line)) {
        if (std::regex_match(line, match, expr)) {
            // Apparently regex array starts from 1 lol
            int tconst = std::stoi(match[2]); // Without tt at the start of the string
            float averageRating = std::stof(match[3]);
            int numVotes = std::stoi(match[4]);

            Record record = { tconst, averageRating, numVotes };
            RecordAddress recordAddress = memPool.writeRecord(record);

            dataset.push_back(std::tuple<int, RecordAddress>(numVotes, recordAddress));

            //if (stopAt == 500000) {
            //    break;
            //}
            //stopAt++;
        }
    }

    std::cout << "================== Experiment 1 ==================" << std::endl;
    std::cout << "Number of records: " << dataset.size() << std::endl;
    std::cout << "Size of a record: " << sizeof(Record) << " bytes" << std::endl;
    std::cout << "Number of records stored in a block: " << std::floor(memPool.getBlockSize() / sizeof(Record)) << std::endl;
    std::cout << "Number of blocks for storing the data: " << memPool.getNumTotalBlocks() - memPool.getNumFreeBlocks() << std::endl;

    // <Block header> <pointer><key><pointer><key><pointer>
    // Need to take care of last pointer, so remove it from available blocksize
    int n = std::floor((blockSize - sizeof(BPTreeNode) - sizeof(BPTreeNode*) - sizeof(int) - sizeof(bool))/(sizeof(BPTreeNode*) + sizeof(int) + sizeof(RecordAddress)));

    std::cout << "Size of n: " << n << "\n";

    BPTree tree(n);
    
    // Insert numVotes into B+ tree
    for(auto data : dataset) {
        tree.insert(data);
    }

    //tree.deleteAllWithKeyValue(6);
    //tree.deleteKey(6);
    //tree.deleteKey(9);
    //tree.deleteKey(29);
    //tree.deleteKey(30);
    //tree.deleteKey(31);
    //tree.deleteKey(32);
    //tree.deleteKey(33);
    //tree.deleteKey(34);
    //tree.deleteKey(35);
    //tree.deleteKey(36);
    //tree.deleteKey(37);
    //tree.deleteKey(38);
    //tree.deleteKey(39);
    //tree.deleteAllWithKeyValue(16);
    //tree.display(tree.getRoot());
    
    std::cout << "================== Experiment 2 ==================" << std::endl;
    // To calculate and print the statistics
    std::cout << "Parameter n of the B+ tree: " << n << std::endl;
    std::cout << "Number of nodes of the B+ tree: " << tree.countNodes() << std::endl; // Implement countNodes in BTree
    std::cout << "Number of levels of the B+ tree: " << tree.treeHeight() << std::endl; // Implement treeHeight in BTree
    std::cout << "Content of the root node (only the keys): ";
    tree.printRootKeys(); // Implement printRootKeys in BTree


    // Experiment 3,4 variables
    double total = 0.0;
    double averageOfAverage = 0.0;
    int bruteAccessCount = 0;
    int numOfIndexNodesAccessed = 0;
    int numOfDataBlocksAccessed = 0;
    std::vector<Record> records;
    // Clock stuff
    // Source for duration counting:
    // https://stackoverflow.com/questions/14391327/how-to-get-duration-as-int-millis-and-float-seconds-from-chrono
    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    auto start = Time::now();
    auto stop = Time::now();
    ms treeDuration;
    ms bruteDuration;

    std::cout << "================== Experiment 3 ==================" << std::endl;
    int numVotes = 500;
    numOfIndexNodesAccessed = 0;
    numOfDataBlocksAccessed = 0;
    records.clear();

    memPool.resetDataBlockAccessCount();
    start = Time::now();
    // Start
    tree.fetchRecordsByRange(tree.getRoot(), numVotes, numVotes, numOfIndexNodesAccessed, memPool, records);
    // Stop
    stop = Time::now();
    treeDuration = std::chrono::duration_cast<ms>(stop - start);
    numOfDataBlocksAccessed = memPool.getDataBlockAccessCount();

    memPool.resetDataBlockAccessCount();
    start = Time::now();
    for (auto data:dataset) {
        RecordAddress address = std::get<1>(data);
        Record record = memPool.readRecord(address);
        int key = record.numVotes;
        if (key == numVotes) {
            // Got key!
        }
    }
    stop = Time::now();
    bruteDuration = std::chrono::duration_cast<ms>(stop - start);
    bruteAccessCount = memPool.getDataBlockAccessCount();

    // To calculate and print the statistics
    total = 0.0;
    averageOfAverage = 0.0;
    for (Record record : records) {
        total += record.averageRating;
    }
    averageOfAverage = total / records.size();
    std::cout << "Number of index nodes the process accesses: " << numOfIndexNodesAccessed << std::endl;
    std::cout << "number of data blocks the process accesses: " << numOfDataBlocksAccessed << std::endl;
    std::cout << "Average of 'averageRating' of the records that are returned: " << averageOfAverage << std::endl;
    std::cout << "Running time of the retrieval process: " << treeDuration.count() << "ms" << std::endl;
    std::cout << "Number of data blocks that would be accessed by a brute-force: " << bruteAccessCount << std::endl;
    std::cout << "running time for brute-force: " << bruteDuration.count() << "ms" << std::endl;

    //tree.display(tree.getRoot());



    std::cout << "================== Experiment 4 ==================" << std::endl;
    int minNumVotes = 30000;
    int maxNumVotes = 40000;
    numOfIndexNodesAccessed = 0;
    numOfDataBlocksAccessed = 0;
    records.clear();

    memPool.resetDataBlockAccessCount();
    start = Time::now();
    // Start
    tree.fetchRecordsByRange(tree.getRoot(), minNumVotes, maxNumVotes, numOfIndexNodesAccessed, memPool, records);
    // Stop
    stop = Time::now();
    treeDuration = std::chrono::duration_cast<ms>(stop - start);
    numOfDataBlocksAccessed = memPool.getDataBlockAccessCount();

    memPool.resetDataBlockAccessCount();
    start = Time::now();
    for (auto data : dataset) {
        RecordAddress address = std::get<1>(data);
        Record record = memPool.readRecord(address);
        int key = record.numVotes;
        if (key >= minNumVotes && key <= maxNumVotes) {
            // Got key!
        }
    }
    stop = Time::now();
    bruteDuration = std::chrono::duration_cast<ms>(stop - start);
    bruteAccessCount = memPool.getDataBlockAccessCount();

    // To calculate and print the statistics
    total = 0.0;
    averageOfAverage = 0.0;
    for (Record record : records) {
        total += record.averageRating;
    }
    averageOfAverage = total / records.size();
    std::cout << "Number of index nodes the process accesses: " << numOfIndexNodesAccessed << std::endl;
    std::cout << "number of data blocks the process accesses: " << numOfDataBlocksAccessed << std::endl;
    std::cout << "Average of 'averageRating' of the records that are returned: " << averageOfAverage << std::endl;
    std::cout << "Running time of the retrieval process: " << treeDuration.count() << "ms" << std::endl;
    std::cout << "Number of data blocks that would be accessed by a brute-force: " << bruteAccessCount << std::endl;
    std::cout << "running time for brute-force: " << bruteDuration.count() << "ms" << std::endl;


    std::cout << "================== Experiment 5 ==================" << std::endl;
}