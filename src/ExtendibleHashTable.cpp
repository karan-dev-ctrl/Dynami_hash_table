#include "ExtendibleHashTable.h"
#include <unordered_set>

ExtendibleHashTable::ExtendibleHashTable(int capacity)
    : globalDepth(1),
      bucketCapacity(capacity),
      keyCount(0),
      splitCount(0) {
    directory.resize(1 << globalDepth);
    directory[0] = std::make_shared<Bucket>(1);
    directory[1] = std::make_shared<Bucket>(1);
}

int ExtendibleHashTable::hashKey(uint32_t key) const {
    uint32_t mask = (1u << globalDepth) - 1u;
    return static_cast<int>(key & mask);
}

bool ExtendibleHashTable::contains(uint32_t key) const {
    int index = hashKey(key);
    const auto& bucket = directory[index];

    for (uint32_t value : bucket->keys) {
        if (value == key) return true;
    }

    return false;
}

bool ExtendibleHashTable::insert(uint32_t key) {
    if (contains(key)) return false;

    while (true) {
        int index  = hashKey(key);
        auto bucket = directory[index];

        if (static_cast<int>(bucket->keys.size()) < bucketCapacity) {
            bucket->keys.push_back(key);
            keyCount++;
            return true;
        }

        if (bucket->localDepth == globalDepth) doubleDirectory();

        splitBucket(index);
    }
}

bool ExtendibleHashTable::remove(uint32_t key) {
    int index  = hashKey(key);
    auto bucket = directory[index];

    for (auto it = bucket->keys.begin(); it != bucket->keys.end(); ++it) {
        if (*it == key) {
            bucket->keys.erase(it);
            keyCount--;
            return true;
        }
    }

    return false;
}

void ExtendibleHashTable::doubleDirectory() {
    int oldSize = static_cast<int>(directory.size());
    directory.resize(oldSize * 2);

    for (int i = 0; i < oldSize; i++) {
        directory[i + oldSize] = directory[i];
    }

    globalDepth++;
}

void ExtendibleHashTable::splitBucket(int dirIndex) {
    auto oldBucket    = directory[dirIndex];
    int oldLocalDepth = oldBucket->localDepth;
    int newLocalDepth = oldLocalDepth + 1;

    auto newBucket        = std::make_shared<Bucket>(newLocalDepth);
    oldBucket->localDepth = newLocalDepth;

    // Only update directory entries that share the same lower oldLocalDepth bits
    // as dirIndex AND have bit oldLocalDepth set — avoids O(directory) scan.
    int lowBits      = dirIndex & ((1 << oldLocalDepth) - 1);
    int patternBit   = 1 << oldLocalDepth;
    int stride       = patternBit << 1;          // skip every other matching entry

    for (int i = lowBits | patternBit;
         static_cast<size_t>(i) < directory.size();
         i += stride) {
        directory[i] = newBucket;
    }

    std::vector<uint32_t> oldKeys = oldBucket->keys;
    oldBucket->keys.clear();

    for (uint32_t key : oldKeys) {
        int idx = hashKey(key);
        directory[idx]->keys.push_back(key);
    }

    splitCount++;
}

int ExtendibleHashTable::getKeyCount() const {
    return keyCount;
}

int ExtendibleHashTable::getSplitCount() const {
    return splitCount;
}

int ExtendibleHashTable::getBucketCount() const {
    std::unordered_set<const Bucket*> unique;
    for (const auto& ptr : directory) unique.insert(ptr.get());
    return static_cast<int>(unique.size());
}

int ExtendibleHashTable::getPageCapacity() const {
    return bucketCapacity;
}

double ExtendibleHashTable::getLoadFactor() const {
    int bucketCount = getBucketCount();
    if (bucketCount == 0 || bucketCapacity == 0) return 0.0;
    return static_cast<double>(keyCount) / (bucketCount * bucketCapacity);
}

size_t ExtendibleHashTable::getMemoryBytes() const {
    // directory pointer array + per-bucket struct + actual key storage
    size_t dirSize    = directory.size() * sizeof(std::shared_ptr<Bucket>);
    size_t bucketMeta = static_cast<size_t>(getBucketCount()) * sizeof(Bucket);
    size_t keyStorage = static_cast<size_t>(keyCount) * sizeof(uint32_t);
    return dirSize + bucketMeta + keyStorage;
}

double ExtendibleHashTable::getPageUtilization() const {
    // count unique buckets that have at least 1 key
    std::unordered_set<const Bucket*> unique;
    for (const auto& ptr : directory) unique.insert(ptr.get());

    int nonEmpty = 0;
    for (const Bucket* b : unique) {
        if (!b->keys.empty()) nonEmpty++;
    }
    if (unique.empty()) return 0.0;
    return static_cast<double>(nonEmpty) / unique.size() * 100.0;
}

void ExtendibleHashTable::print() const {
    std::cout << "\n--- Extendible Hash Table ---\n";
    std::cout << "Global Depth : " << globalDepth     << "\n";
    std::cout << "Total Keys   : " << keyCount         << "\n";
    std::cout << "Buckets      : " << getBucketCount() << "\n";
    std::cout << "Splits       : " << splitCount       << "\n";
    std::cout << "Load Factor  : " << getLoadFactor()  << "\n";
    std::cout << "Memory       : " << getMemoryBytes() / 1024.0 / 1024.0 << " MB\n";
    std::cout << "-----------------------------\n";
}
