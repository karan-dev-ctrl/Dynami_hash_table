#include "LarsonDynamicHashTable.h"

LarsonDynamicHashTable::LarsonDynamicHashTable(int initBuckets, int capacity,
                                               double maxLoad, double minLoad)
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

int LarsonDynamicHashTable::getBucketIndex(uint32_t key) const {
    int base  = initialBuckets * (1 << level);
    int index = static_cast<int>(key % static_cast<uint32_t>(base));

    if (index < nextSplit) {
        index = static_cast<int>(key % static_cast<uint32_t>(2 * base));
    }

    return index;
}

bool LarsonDynamicHashTable::contains(uint32_t key) const {
    int index = getBucketIndex(key);

    for (uint32_t value : buckets[index]) {
        if (value == key) return true;
    }

    return false;
}

bool LarsonDynamicHashTable::insert(uint32_t key) {
    if (contains(key)) return false;

    int index = getBucketIndex(key);
    buckets[index].push_back(key);
    keyCount++;

    if (getLoadFactor() > maxLoadFactor) split();

    return true;
}

bool LarsonDynamicHashTable::remove(uint32_t key) {
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
    int base           = initialBuckets * (1 << level);
    int splitIndex     = nextSplit;
    int newBucketIndex = splitIndex + base;

    buckets.push_back(std::vector<uint32_t>());

    std::vector<uint32_t> oldKeys = buckets[splitIndex];
    buckets[splitIndex].clear();

    for (uint32_t key : oldKeys) {
        int newIndex = static_cast<int>(key % static_cast<uint32_t>(2 * base));

        if (newIndex == splitIndex)
            buckets[splitIndex].push_back(key);
        else
            buckets[newBucketIndex].push_back(key);
    }

    nextSplit++;
    splitCount++;

    if (nextSplit == base) {
        nextSplit = 0;
        level++;
    }
}

void LarsonDynamicHashTable::merge() {
    if (buckets.size() <= static_cast<size_t>(initialBuckets)) return;

    int base = initialBuckets * (1 << level);

    if (nextSplit == 0) {
        if (level == 0) return;
        level--;
        base      = initialBuckets * (1 << level);
        nextSplit = base;
    }

    nextSplit--;
    int buddyIndex = nextSplit + base;

    if (buddyIndex >= static_cast<int>(buckets.size())) return;

    for (uint32_t key : buckets[buddyIndex]) {
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
    if (buckets.empty() || bucketCapacity == 0) return 0.0;
    return static_cast<double>(keyCount) / (buckets.size() * bucketCapacity);
}

size_t LarsonDynamicHashTable::getMemoryBytes() const {
    size_t bucketOverhead = buckets.size() * sizeof(std::vector<uint32_t>);
    size_t keyStorage     = static_cast<size_t>(keyCount) * sizeof(uint32_t);
    return bucketOverhead + keyStorage;
}

double LarsonDynamicHashTable::getPageUtilization() const {
    if (buckets.empty()) return 0.0;
    int nonEmpty = 0;
    for (const auto& bucket : buckets) {
        if (!bucket.empty()) nonEmpty++;
    }
    return static_cast<double>(nonEmpty) / buckets.size() * 100.0;
}

void LarsonDynamicHashTable::print() const {
    std::cout << "\n--- Larson Dynamic Hash Table ---\n";
    std::cout << "Level       : " << level           << "\n";
    std::cout << "Next Split  : " << nextSplit        << "\n";
    std::cout << "Total Keys  : " << keyCount         << "\n";
    std::cout << "Buckets     : " << getBucketCount() << "\n";
    std::cout << "Splits      : " << splitCount       << "\n";
    std::cout << "Load Factor : " << getLoadFactor()  << "\n";
    std::cout << "Memory      : " << getMemoryBytes() / 1024.0 / 1024.0 << " MB\n";
    std::cout << "---------------------------------\n";
}
