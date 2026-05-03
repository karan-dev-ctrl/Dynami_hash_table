#include "LarsonDynamicHashTable.h"

LarsonDynamicHashTable::LarsonDynamicHashTable(int initBuckets,
                                               int capacity,
                                               double maxLoad,
                                               double minLoad)
    : initialBuckets(initBuckets),
      bucketCapacity(capacity),
      level(0),
      nextSplit(0),
      keyCount(0),
      splitCount(0),
      maxLoadFactor(maxLoad),
      minLoadFactor(minLoad) {
    buckets.resize(initialBuckets);
}

int LarsonDynamicHashTable::getBucketIndex(int key) const {
    int base = initialBuckets * (1 << level);
    int index = key % base;

    if (index < nextSplit) {
        index = key % (2 * base);
    }

    return index;
}

bool LarsonDynamicHashTable::contains(int key) const {
    int index = getBucketIndex(key);

    for (int value : buckets[index]) {
        if (value == key) {
            return true;
        }
    }
    return false;
}

bool LarsonDynamicHashTable::insert(int key) {
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

bool LarsonDynamicHashTable::remove(int key) {
    int index = getBucketIndex(key);

    for (auto it = buckets[index].begin(); it != buckets[index].end(); ++it) {
        if (*it == key) {
            buckets[index].erase(it);
            keyCount--;

            if (getLoadFactor() < minLoadFactor &&
                static_cast<int>(buckets.size()) > initialBuckets) {
                merge();
            }

            return true;
        }
    }

    return false;
}

void LarsonDynamicHashTable::split() {
    int base = initialBuckets * (1 << level);
    int splitIndex = nextSplit;
    int newBucketIndex = splitIndex + base;

    buckets.push_back(std::vector<int>());

    std::vector<int> oldKeys = buckets[splitIndex];
    buckets[splitIndex].clear();

    for (int key : oldKeys) {
        int newIndex = key % (2 * base);

        if (newIndex == splitIndex) {
            buckets[splitIndex].push_back(key);
        } else {
            buckets[newBucketIndex].push_back(key);
        }
    }

    nextSplit++;
    splitCount++;

    if (nextSplit == base) {
        nextSplit = 0;
        level++;
    }
}

void LarsonDynamicHashTable::merge() {
    if (buckets.size() <= static_cast<size_t>(initialBuckets)) {
        return;
    }

    int base = initialBuckets * (1 << level);

    if (nextSplit == 0) {
        if (level == 0) {
            return;
        }
        level--;
        base = initialBuckets * (1 << level);
        nextSplit = base;
    }

    nextSplit--;
    int buddyIndex = nextSplit + base;

    if (buddyIndex >= static_cast<int>(buckets.size())) {
        return;
    }

    for (int key : buckets[buddyIndex]) {
        buckets[nextSplit].push_back(key);
    }

    buckets.pop_back();
}

int LarsonDynamicHashTable::getKeyCount() const {
    return keyCount;
}

int LarsonDynamicHashTable::getBucketCount() const {
    return static_cast<int>(buckets.size());
}

int LarsonDynamicHashTable::getSplitCount() const {
    return splitCount;
}

double LarsonDynamicHashTable::getLoadFactor() const {
    if (buckets.empty() || bucketCapacity == 0) {
        return 0.0;
    }

    return static_cast<double>(keyCount) / (buckets.size() * bucketCapacity);
}

void LarsonDynamicHashTable::print() const {
    std::cout << "\n--- Larson Dynamic Hash Table ---\n";
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

    std::cout << "---------------------------------\n";
}