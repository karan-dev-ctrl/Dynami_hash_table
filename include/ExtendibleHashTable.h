#ifndef EXTENDIBLEHASHTABLE_H
#define EXTENDIBLEHASHTABLE_H

#include <cstdint>
#include <vector>
#include <memory>
#include <iostream>

class ExtendibleHashTable {
private:
    struct Bucket {
        int localDepth;
        std::vector<uint32_t> keys;

        Bucket(int depth) : localDepth(depth) {}
    };

    std::vector<std::shared_ptr<Bucket>> directory;
    int    globalDepth;
    int    bucketCapacity;
    int    keyCount;
    int    splitCount;

    int  hashKey(uint32_t key) const;
    void doubleDirectory();
    void splitBucket(int dirIndex);

public:
    ExtendibleHashTable(int capacity = 4);

    bool insert(uint32_t key);
    bool contains(uint32_t key) const;
    bool remove(uint32_t key);

    int    getKeyCount()    const;
    int    getBucketCount() const;
    int    getSplitCount()  const;
    double getLoadFactor()  const;
    size_t getMemoryBytes() const;

    void print() const;
};

#endif
