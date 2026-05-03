#ifndef LINEARHASHTABLE_H
#define LINEARHASHTABLE_H

#include <vector>
#include <iostream>


class LinearHashTable {
private:
    std::vector<std::vector<int>> buckets;

    int initialBuckets;
    int bucketCapacity;
    int level;
    int nextSplit;
    int keyCount;
    int splitCount;

    double maxLoadFactor;

    int getBucketIndex(int key) const;
    void split();

public:
    LinearHashTable(int initBuckets = 2, int capacity = 2, double maxLoad = 0.75);

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