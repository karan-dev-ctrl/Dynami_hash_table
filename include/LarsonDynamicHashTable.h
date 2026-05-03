#ifndef LARSONDYNAMICHASHTABLE_H
#define LARSONDYNAMICHASHTABLE_H

#include <vector>
#include <iostream>

class LarsonDynamicHashTable {
private:
    std::vector<std::vector<int>> buckets;

    int initialBuckets;
    int bucketCapacity;
    int level;
    int nextSplit;
    int keyCount;
    int splitCount;

    double maxLoadFactor;
    double minLoadFactor;

    int getBucketIndex(int key) const;
    void split();
    void merge();

public:
    LarsonDynamicHashTable(int initBuckets = 2,
                           int capacity = 2,
                           double maxLoad = 0.75,
                           double minLoad = 0.25);

    bool insert(int key);
    bool contains(int key) const;
    bool remove(int key);

    int getKeyCount() const;
    int getBucketCount() const;
    int getSplitCount() const;
    double getLoadFactor() const;

    void print() const;
};

#endif