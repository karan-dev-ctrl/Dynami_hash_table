#ifndef EXTENDIBLEHASHTABLE_H
#define EXTENDIBLEHASHTABLE_H

#include <vector>
#include <memory>
#include <iostream>

class ExtendibleHashTable {
private:
    struct Bucket {
        int localDepth;
        std::vector<int> keys;

        Bucket(int depth) : localDepth(depth) {}
    };

    std::vector<std::shared_ptr<Bucket>> directory;
    int globalDepth;
    int bucketCapacity;
    int keyCount;
    int splitCount;

    int hashKey(int key) const;
    void doubleDirectory();
    void splitBucket(int dirIndex);

public:
    ExtendibleHashTable(int capacity = 2);

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