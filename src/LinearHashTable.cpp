#include "LinearHashTable.h"

LinearHashTable::LinearHashTable(int initBuckets, int capacity, double maxLoad)
    : initialBuckets(initBuckets),
      bucketCapacity(capacity),
      level(0),
      nextSplit(0),
      keyCount(0),
      splitCount(0),
      maxLoadFactor(maxLoad) {
    buckets.resize(initialBuckets);
}

int LinearHashTable::getBucketIndex(int key) const {
    int mod = initialBuckets * (1 << level);
    int index = key % mod;

    if (index < nextSplit) {
        mod = initialBuckets * (1 << (level + 1));
        index = key % mod;
    }

    return index;
}

bool LinearHashTable::contains(int key) const {
    int index = getBucketIndex(key);

    for (int value : buckets[index]) {
        if (value == key) {
            return true;
        }
    }

    return false;
}

bool LinearHashTable::insert(int key) {
    if (contains(key)) {
        return false;
    }

    int index = getBucketIndex(key);
    buckets[index].push_back(key);
    keyCount++;

    if (getLoadFactor() > maxLoadFactor) {
        split();
    }

    return true;
}

bool LinearHashTable::remove(int key) {
    int index = getBucketIndex(key);

    for (auto it = buckets[index].begin(); it != buckets[index].end(); ++it) {
        if (*it == key) {
            buckets[index].erase(it);
            keyCount--;
            return true;
        }
    }

    return false;
}

void LinearHashTable::split() {
    int splitIndex = nextSplit;
    int oldBase = initialBuckets * (1 << level);
    int newBucketIndex = splitIndex + oldBase;

    buckets.push_back(std::vector<int>());

    std::vector<int> oldKeys = buckets[splitIndex];
    buckets[splitIndex].clear();

    for (int key : oldKeys) {
        int newIndex = key % (2 * oldBase);

        if (newIndex == splitIndex) {
            buckets[splitIndex].push_back(key);
        } else {
            buckets[newBucketIndex].push_back(key);
        }
    }

    nextSplit++;
    splitCount++;

    if (nextSplit == oldBase) {
        nextSplit = 0;
        level++;
    }
}

int LinearHashTable::getKeyCount() const {
    return keyCount;
}

int LinearHashTable::getBucketCount() const {
    return static_cast<int>(buckets.size());
}

int LinearHashTable::getSplitCount() const {
    return splitCount;
}

double LinearHashTable::getLoadFactor() const {
    if (buckets.empty() || bucketCapacity == 0) {
        return 0.0;
    }

    return static_cast<double>(keyCount) / (buckets.size() * bucketCapacity);
}

void LinearHashTable::print() const {
    std::cout << "\n--- Linear Hash Table ---\n";
    std::cout << "Level: " << level << "\n";
    std::cout << "Next Split: " << nextSplit << "\n";
    std::cout << "Total Keys: " << keyCount << "\n";
    std::cout << "Bucket Count: " << getBucketCount() << "\n";
    std::cout << "Split Count: " << splitCount << "\n";
    std::cout << "Load Factor: " << getLoadFactor() << "\n";

    for (size_t i = 0; i < buckets.size(); i++) {
        std::cout << "Bucket " << i << ": ";
        for (int key : buckets[i]) {
            std::cout << key << " ";
        }
        std::cout << "\n";
    }

    std::cout << "-------------------------\n";
}