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

int LinearHashTable::getBucketIndex(uint32_t key) const {
    int mod   = initialBuckets * (1 << level);
    int index = static_cast<int>(key % static_cast<uint32_t>(mod));

    if (index < nextSplit) {
        mod   = initialBuckets * (1 << (level + 1));
        index = static_cast<int>(key % static_cast<uint32_t>(mod));
    }

    return index;
}

bool LinearHashTable::contains(uint32_t key) const {
    int index = getBucketIndex(key);

    for (uint32_t value : buckets[index]) {
        if (value == key) return true;
    }

    return false;
}

bool LinearHashTable::insert(uint32_t key) {
    if (contains(key)) return false;

    int index = getBucketIndex(key);
    buckets[index].push_back(key);
    keyCount++;

    if (getLoadFactor() > maxLoadFactor) split();

    return true;
}

bool LinearHashTable::remove(uint32_t key) {
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
    int splitIndex    = nextSplit;
    int oldBase       = initialBuckets * (1 << level);
    int newBucketIndex = splitIndex + oldBase;

    buckets.push_back(std::vector<uint32_t>());

    std::vector<uint32_t> oldKeys = buckets[splitIndex];
    buckets[splitIndex].clear();

    for (uint32_t key : oldKeys) {
        int newIndex = static_cast<int>(key % static_cast<uint32_t>(2 * oldBase));

        if (newIndex == splitIndex)
            buckets[splitIndex].push_back(key);
        else
            buckets[newBucketIndex].push_back(key);
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

int LinearHashTable::getPageCapacity() const {
    return bucketCapacity;
}

int LinearHashTable::getSplitCount() const {
    return splitCount;
}

double LinearHashTable::getLoadFactor() const {
    if (buckets.empty() || bucketCapacity == 0) return 0.0;
    return static_cast<double>(keyCount) / (buckets.size() * bucketCapacity);
}

size_t LinearHashTable::getMemoryBytes() const {
    // outer vector entries + actual key storage
    size_t bucketOverhead = buckets.size() * sizeof(std::vector<uint32_t>);
    size_t keyStorage     = static_cast<size_t>(keyCount) * sizeof(uint32_t);
    return bucketOverhead + keyStorage;
}

double LinearHashTable::getPageUtilization() const {
    if (buckets.empty()) return 0.0;
    int nonEmpty = 0;
    for (const auto& bucket : buckets) {
        if (!bucket.empty()) nonEmpty++;
    }
    return static_cast<double>(nonEmpty) / buckets.size() * 100.0;
}

void LinearHashTable::print() const {
    std::cout << "\n--- Linear Hash Table ---\n";
    std::cout << "Level       : " << level          << "\n";
    std::cout << "Next Split  : " << nextSplit       << "\n";
    std::cout << "Total Keys  : " << keyCount        << "\n";
    std::cout << "Buckets     : " << getBucketCount() << "\n";
    std::cout << "Splits      : " << splitCount      << "\n";
    std::cout << "Load Factor : " << getLoadFactor() << "\n";
    std::cout << "Memory      : " << getMemoryBytes() / 1024.0 / 1024.0 << " MB\n";
    std::cout << "-------------------------\n";
}
