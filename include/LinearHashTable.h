#ifndef LINEARHASHTABLE_H
#define LINEARHASHTABLE_H

#include <cstdint>
#include <vector>
#include <iostream>

class LinearHashTable {
private:
    std::vector<std::vector<uint32_t>> buckets;

    int    initialBuckets;
    int    bucketCapacity;
    int    level;
    int    nextSplit;
    int    keyCount;
    int    splitCount;

    double maxLoadFactor;

    int  getBucketIndex(uint32_t key) const;
    void split();

public:
    LinearHashTable(int initBuckets = 2, int capacity = 4, double maxLoad = 0.75);

    bool insert(uint32_t key);
    bool contains(uint32_t key) const;
    bool remove(uint32_t key);

    int    getKeyCount()         const;
    int    getBucketCount()      const;
    int    getSplitCount()       const;
    int    getPageCapacity()     const;
    double getLoadFactor()       const;
    size_t getMemoryBytes()      const;
    double getPageUtilization()  const;

    void print() const;
};

#endif
