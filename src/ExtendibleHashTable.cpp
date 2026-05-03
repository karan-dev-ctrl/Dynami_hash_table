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

int ExtendibleHashTable::hashKey(int key) const {
    int mask = (1 << globalDepth) - 1;
    return key & mask;
}

bool ExtendibleHashTable::contains(int key) const {
    int index = hashKey(key);
    const auto& bucket = directory[index];

    for (int value : bucket->keys) {
        if (value == key) {
            return true;
        }
    }
    return false;
}

bool ExtendibleHashTable::insert(int key) {
    if (contains(key)) {
        return false;
    }

    while (true) {
        int index = hashKey(key);
        auto bucket = directory[index];

        if ((int)bucket->keys.size() < bucketCapacity) {
            bucket->keys.push_back(key);
            keyCount++;
            return true;
        }

        if (bucket->localDepth == globalDepth) {
            doubleDirectory();
        }

        splitBucket(index);
    }
}

bool ExtendibleHashTable::remove(int key) {
    int index = hashKey(key);
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
    int oldSize = directory.size();
    directory.resize(oldSize * 2);

    for (int i = 0; i < oldSize; i++) {
        directory[i + oldSize] = directory[i];
    }

    globalDepth++;
}

void ExtendibleHashTable::splitBucket(int dirIndex) {
    auto oldBucket = directory[dirIndex];
    int oldLocalDepth = oldBucket->localDepth;
    int newLocalDepth = oldLocalDepth + 1;

    auto newBucket = std::make_shared<Bucket>(newLocalDepth);
    oldBucket->localDepth = newLocalDepth;

    int patternBit = 1 << oldLocalDepth;

    for (size_t i = 0; i < directory.size(); i++) {
        if (directory[i] == oldBucket && (static_cast<int>(i) & patternBit)) {
            directory[i] = newBucket;
        }
    }

    std::vector<int> oldKeys = oldBucket->keys;
    oldBucket->keys.clear();

    for (int key : oldKeys) {
        int index = hashKey(key);
        directory[index]->keys.push_back(key);
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
    std::unordered_set<const Bucket*> uniqueBuckets;
    for (const auto& ptr : directory) {
        uniqueBuckets.insert(ptr.get());
    }
    return static_cast<int>(uniqueBuckets.size());
}

double ExtendibleHashTable::getLoadFactor() const {
    int bucketCount = getBucketCount();
    if (bucketCount == 0 || bucketCapacity == 0) {
        return 0.0;
    }

    return static_cast<double>(keyCount) / (bucketCount * bucketCapacity);
}

void ExtendibleHashTable::print() const {
    std::cout << "\n--- Extendible Hash Table ---\n";
    std::cout << "Global Depth: " << globalDepth << "\n";
    std::cout << "Total Keys: " << keyCount << "\n";
    std::cout << "Bucket Count: " << getBucketCount() << "\n";
    std::cout << "Split Count: " << splitCount << "\n";
    std::cout << "Load Factor: " << getLoadFactor() << "\n";

    std::cout << "\nDirectory:\n";
    for (size_t i = 0; i < directory.size(); i++) {
        std::cout << "Dir[" << i << "] -> Bucket@" << directory[i].get()
                  << " (ld=" << directory[i]->localDepth << ")\n";
    }

    std::cout << "\nUnique Buckets:\n";
    std::vector<const Bucket*> printed;

    for (const auto& bucketPtr : directory) {
        const Bucket* rawPtr = bucketPtr.get();

        bool alreadyPrinted = false;
        for (const Bucket* ptr : printed) {
            if (ptr == rawPtr) {
                alreadyPrinted = true;
                break;
            }
        }

        if (!alreadyPrinted) {
            printed.push_back(rawPtr);

            std::cout << "Bucket@" << rawPtr
                      << " (localDepth=" << bucketPtr->localDepth << "): ";

            for (int key : bucketPtr->keys) {
                std::cout << key << " ";
            }
            std::cout << "\n";
        }
    }

    std::cout << "-----------------------------\n";
}