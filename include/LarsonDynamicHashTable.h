#ifndef LARSONDYNAMICHASHTABLE_H
#define LARSONDYNAMICHASHTABLE_H

#include <cstdint>
#include <vector>
#include <iostream>

class LarsonDynamicHashTable {
private:
    std::vector<std::vector<uint32_t>> buckets;

    int    initialBuckets;
    int    bucketCapacity;
    int    level;
    int    nextSplit;
    int    keyCount;
    int    splitCount;

    double maxLoadFactor;
    double minLoadFactor;

    int  getBucketIndex(uint32_t key) const;
    void split();
    void merge();

public:
    LarsonDynamicHashTable(int    initBuckets = 2,
                           int    capacity    = 4,
                           double maxLoad     = 0.75,
                           double minLoad     = 0.25);

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
