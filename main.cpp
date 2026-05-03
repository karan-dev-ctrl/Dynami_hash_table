// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>
// #include <chrono>
// #include "LinearHashTable.h"

// int main() {
//     std::srand(static_cast<unsigned>(std::time(nullptr)));

//     LinearHashTable table(2, 2, 0.75);

//     bool benchmarkMode = true;

//     if (!benchmarkMode) {
//         const int n = 20;
//         std::vector<int> keys;

//         std::cout << "Random keys:\n";
//         for (int i = 0; i < n; i++) {
//             int key = std::rand() % 100;
//             keys.push_back(key);
//             std::cout << key << " ";
//         }
//         std::cout << "\n\n";

//         for (int key : keys) {
//             std::cout << "Inserting " << key;
//             bool inserted = table.insert(key);

//             if (!inserted) {
//                 std::cout << " (duplicate ignored)";
//             }
//             std::cout << "\n";
//         }

//         table.print();
//     } else {
//         const int n = 1000;
//         std::vector<int> keys;
//         keys.reserve(n);

//         for (int i = 0; i < n; i++) {
//             keys.push_back(std::rand() % 5000);
//         }

//         auto startInsert = std::chrono::high_resolution_clock::now();

//         for (int key : keys) {
//             table.insert(key);
//         }

//         auto endInsert = std::chrono::high_resolution_clock::now();
//         auto insertTime = std::chrono::duration_cast<std::chrono::milliseconds>(endInsert - startInsert);

//         auto startSearch = std::chrono::high_resolution_clock::now();

//         int foundCount = 0;
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }

//         auto endSearch = std::chrono::high_resolution_clock::now();
//         auto searchTime = std::chrono::duration_cast<std::chrono::milliseconds>(endSearch - startSearch);

//         std::cout << "\n--- Benchmark Results ---\n";
//         std::cout << "Inserted keys attempted: " << n << "\n";
//         std::cout << "Unique keys stored: " << table.getKeyCount() << "\n";
//         std::cout << "Buckets: " << table.getBucketCount() << "\n";
//         std::cout << "Splits: " << table.getSplitCount() << "\n";
//         std::cout << "Load Factor: " << table.getLoadFactor() << "\n";
//         std::cout << "Successful searches: " << foundCount << "\n";
//         std::cout << "Insertion time: " << insertTime.count() << " ms\n";
//         std::cout << "Search time: " << searchTime.count() << " ms\n";
//     }

//     return 0;
// }


// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>
// #include <chrono>
// #include "ExtendibleHashTable.h"

// int main() {
//     std::srand(static_cast<unsigned>(std::time(nullptr)));

//     ExtendibleHashTable table(2);

//     bool benchmarkMode = false;

//     if (!benchmarkMode) {
//         const int n = 20;
//         std::vector<int> keys;

//         std::cout << "Random keys:\n";
//         for (int i = 0; i < n; i++) {
//             int key = std::rand() % 100;
//             keys.push_back(key);
//             std::cout << key << " ";
//         }
//         std::cout << "\n\n";

//         for (int key : keys) {
//             std::cout << "Inserting " << key;
//             bool inserted = table.insert(key);

//             if (!inserted) {
//                 std::cout << " (duplicate ignored)";
//             }
//             std::cout << "\n";
//         }

//         table.print();

//         if (!keys.empty()) {
//             int x = keys[0];
//             std::cout << "\nContains " << x << ": " << (table.contains(x) ? "Yes" : "No") << "\n";
//             std::cout << "Removing " << x << ": " << (table.remove(x) ? "Removed" : "Not Found") << "\n";
//             std::cout << "Contains " << x << ": " << (table.contains(x) ? "Yes" : "No") << "\n";
//         }
//     } else {
//         const int n = 1000;
//         std::vector<int> keys;
//         keys.reserve(n);

//         for (int i = 0; i < n; i++) {
//             keys.push_back(std::rand() % 5000);
//         }

//         auto startInsert = std::chrono::high_resolution_clock::now();
//         for (int key : keys) {
//             table.insert(key);
//         }
//         auto endInsert = std::chrono::high_resolution_clock::now();

//         auto startSearch = std::chrono::high_resolution_clock::now();
//         int foundCount = 0;
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//         auto endSearch = std::chrono::high_resolution_clock::now();

//         auto insertTime = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert);
//         auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch);

//         std::cout << "\n--- Extendible Hashing Benchmark ---\n";
//         std::cout << "Inserted keys attempted: " << n << "\n";
//         std::cout << "Unique keys stored: " << table.getKeyCount() << "\n";
//         std::cout << "Buckets: " << table.getBucketCount() << "\n";
//         std::cout << "Splits: " << table.getSplitCount() << "\n";
//         std::cout << "Load Factor: " << table.getLoadFactor() << "\n";
//         std::cout << "Successful searches: " << foundCount << "\n";
//         std::cout << "Insertion time: " << insertTime.count() << " us\n";
//         std::cout << "Search time: " << searchTime.count() << " us\n";
//     }

//     return 0;
// }


// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>
// #include <chrono>

// #include "LinearHashTable.h"
// #include "ExtendibleHashTable.h"

// void runLinearHashingTest(const std::vector<int>& keys) {
//     LinearHashTable table(2, 2, 0.75);

//     auto startInsert = std::chrono::high_resolution_clock::now();
//     for (int key : keys) {
//         table.insert(key);
//     }
//     auto endInsert = std::chrono::high_resolution_clock::now();

//     auto startSearch = std::chrono::high_resolution_clock::now();
//     int foundCount = 0;

//     for (int round = 0; round < 100; round++) {
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//     }

//     auto endSearch = std::chrono::high_resolution_clock::now();

//     auto insertTime = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert);
//     auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch);

//     std::cout << "\n=== Linear Hashing Results ===\n";
//     std::cout << "Inserted keys attempted: " << keys.size() << "\n";
//     std::cout << "Unique keys stored: " << table.getKeyCount() << "\n";
//     std::cout << "Buckets: " << table.getBucketCount() << "\n";
//     std::cout << "Splits: " << table.getSplitCount() << "\n";
//     std::cout << "Load Factor: " << table.getLoadFactor() << "\n";
//     std::cout << "Successful searches: " << foundCount << "\n";
//     std::cout << "Insertion time: " << insertTime.count() << " us\n";
//     std::cout << "Search time: " << searchTime.count() << " us\n";
// }

// void runExtendibleHashingTest(const std::vector<int>& keys) {
//     ExtendibleHashTable table(2);

//     auto startInsert = std::chrono::high_resolution_clock::now();
//     for (int key : keys) {
//         table.insert(key);
//     }
//     auto endInsert = std::chrono::high_resolution_clock::now();

//     auto startSearch = std::chrono::high_resolution_clock::now();
//     int foundCount = 0;

//     for (int round = 0; round < 100; round++) {
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//     }

//     auto endSearch = std::chrono::high_resolution_clock::now();

//     auto insertTime = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert);
//     auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch);

//     std::cout << "\n=== Extendible Hashing Results ===\n";
//     std::cout << "Inserted keys attempted: " << keys.size() << "\n";
//     std::cout << "Unique keys stored: " << table.getKeyCount() << "\n";
//     std::cout << "Buckets: " << table.getBucketCount() << "\n";
//     std::cout << "Splits: " << table.getSplitCount() << "\n";
//     std::cout << "Load Factor: " << table.getLoadFactor() << "\n";
//     std::cout << "Successful searches: " << foundCount << "\n";
//     std::cout << "Insertion time: " << insertTime.count() << " us\n";
//     std::cout << "Search time: " << searchTime.count() << " us\n";
// }

// int main() {
//     std::srand(static_cast<unsigned>(std::time(nullptr)));

//     const int n = 1000;
//     std::vector<int> keys;
//     keys.reserve(n);

//     for (int i = 0; i < n; i++) {
//         keys.push_back(std::rand() % 5000);
//     }

//     std::cout << "=====================================\n";
//     std::cout << " Hashing Comparison on Same Dataset\n";
//     std::cout << "=====================================\n";
//     std::cout << "Dataset size: " << n << "\n";

//     runLinearHashingTest(keys);
//     runExtendibleHashingTest(keys);

//     return 0;
// }


// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>
// #include <chrono>
// #include "LarsonDynamicHashTable.h"

// int main() {
//     std::srand(static_cast<unsigned>(std::time(nullptr)));

//     LarsonDynamicHashTable table(2, 2, 0.75, 0.25);

//     bool benchmarkMode = false;

//     if (!benchmarkMode) {
//         const int n = 20;
//         std::vector<int> keys;

//         std::cout << "Random keys:\n";
//         for (int i = 0; i < n; i++) {
//             int key = std::rand() % 100;
//             keys.push_back(key);
//             std::cout << key << " ";
//         }
//         std::cout << "\n\n";

//         for (int key : keys) {
//             std::cout << "Inserting " << key;
//             bool inserted = table.insert(key);

//             if (!inserted) {
//                 std::cout << " (duplicate ignored)";
//             }
//             std::cout << "\n";
//         }

//         table.print();

//         if (!keys.empty()) {
//             int x = keys[0];
//             std::cout << "\nContains " << x << ": " << (table.contains(x) ? "Yes" : "No") << "\n";
//             std::cout << "Removing " << x << ": " << (table.remove(x) ? "Removed" : "Not Found") << "\n";
//             std::cout << "Contains " << x << ": " << (table.contains(x) ? "Yes" : "No") << "\n";
//             table.print();
//         }
//     } else {
//         const int n = 1000;
//         std::vector<int> keys;
//         keys.reserve(n);

//         for (int i = 0; i < n; i++) {
//             keys.push_back(std::rand() % 5000);
//         }

//         auto startInsert = std::chrono::high_resolution_clock::now();
//         for (int key : keys) {
//             table.insert(key);
//         }
//         auto endInsert = std::chrono::high_resolution_clock::now();

//         auto startSearch = std::chrono::high_resolution_clock::now();
//         int foundCount = 0;
//         for (int round = 0; round < 100; round++) {
//             for (int key : keys) {
//                 if (table.contains(key)) {
//                     foundCount++;
//                 }
//             }
//         }
//         auto endSearch = std::chrono::high_resolution_clock::now();

//         auto insertTime = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert);
//         auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch);

//         std::cout << "\n--- Larson Benchmark Results ---\n";
//         std::cout << "Inserted keys attempted: " << n << "\n";
//         std::cout << "Unique keys stored: " << table.getKeyCount() << "\n";
//         std::cout << "Buckets: " << table.getBucketCount() << "\n";
//         std::cout << "Splits: " << table.getSplitCount() << "\n";
//         std::cout << "Load Factor: " << table.getLoadFactor() << "\n";
//         std::cout << "Successful searches: " << foundCount << "\n";
//         std::cout << "Insertion time: " << insertTime.count() << " us\n";
//         std::cout << "Search time: " << searchTime.count() << " us\n";
//     }

//     return 0;
// }



// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>
// #include <chrono>

// #include "LinearHashTable.h"
// #include "ExtendibleHashTable.h"
// #include "LarsonDynamicHashTable.h"

// void runLinearHashingTest(const std::vector<int>& keys) {
//     LinearHashTable table(2, 2, 0.75);

//     auto startInsert = std::chrono::high_resolution_clock::now();
//     for (int key : keys) {
//         table.insert(key);
//     }
//     auto endInsert = std::chrono::high_resolution_clock::now();

//     auto startSearch = std::chrono::high_resolution_clock::now();
//     int foundCount = 0;
//     for (int round = 0; round < 100; round++) {
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//     }
//     auto endSearch = std::chrono::high_resolution_clock::now();

//     auto insertTime = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert);
//     auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch);

//     std::cout << "\n=== Linear Hashing Results ===\n";
//     std::cout << "Inserted keys attempted: " << keys.size() << "\n";
//     std::cout << "Unique keys stored: " << table.getKeyCount() << "\n";
//     std::cout << "Buckets: " << table.getBucketCount() << "\n";
//     std::cout << "Splits: " << table.getSplitCount() << "\n";
//     std::cout << "Load Factor: " << table.getLoadFactor() << "\n";
//     std::cout << "Successful searches: " << foundCount << "\n";
//     std::cout << "Insertion time: " << insertTime.count() << " us\n";
//     std::cout << "Search time: " << searchTime.count() << " us\n";
// }

// void runExtendibleHashingTest(const std::vector<int>& keys) {
//     ExtendibleHashTable table(2);

//     auto startInsert = std::chrono::high_resolution_clock::now();
//     for (int key : keys) {
//         table.insert(key);
//     }
//     auto endInsert = std::chrono::high_resolution_clock::now();

//     auto startSearch = std::chrono::high_resolution_clock::now();
//     int foundCount = 0;
//     for (int round = 0; round < 100; round++) {
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//     }
//     auto endSearch = std::chrono::high_resolution_clock::now();

//     auto insertTime = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert);
//     auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch);

//     std::cout << "\n=== Extendible Hashing Results ===\n";
//     std::cout << "Inserted keys attempted: " << keys.size() << "\n";
//     std::cout << "Unique keys stored: " << table.getKeyCount() << "\n";
//     std::cout << "Buckets: " << table.getBucketCount() << "\n";
//     std::cout << "Splits: " << table.getSplitCount() << "\n";
//     std::cout << "Load Factor: " << table.getLoadFactor() << "\n";
//     std::cout << "Successful searches: " << foundCount << "\n";
//     std::cout << "Insertion time: " << insertTime.count() << " us\n";
//     std::cout << "Search time: " << searchTime.count() << " us\n";
// }

// void runLarsonHashingTest(const std::vector<int>& keys) {
//     LarsonDynamicHashTable table(2, 2, 0.75, 0.25);

//     auto startInsert = std::chrono::high_resolution_clock::now();
//     for (int key : keys) {
//         table.insert(key);
//     }
//     auto endInsert = std::chrono::high_resolution_clock::now();

//     auto startSearch = std::chrono::high_resolution_clock::now();
//     int foundCount = 0;
//     for (int round = 0; round < 100; round++) {
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//     }
//     auto endSearch = std::chrono::high_resolution_clock::now();

//     auto insertTime = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert);
//     auto searchTime = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch);

//     std::cout << "\n=== Larson Dynamic Hash Table Results ===\n";
//     std::cout << "Inserted keys attempted: " << keys.size() << "\n";
//     std::cout << "Unique keys stored: " << table.getKeyCount() << "\n";
//     std::cout << "Buckets: " << table.getBucketCount() << "\n";
//     std::cout << "Splits: " << table.getSplitCount() << "\n";
//     std::cout << "Load Factor: " << table.getLoadFactor() << "\n";
//     std::cout << "Successful searches: " << foundCount << "\n";
//     std::cout << "Insertion time: " << insertTime.count() << " us\n";
//     std::cout << "Search time: " << searchTime.count() << " us\n";
// }

// int main() {
//     std::srand(static_cast<unsigned>(std::time(nullptr)));

//     const int n = 1000;
//     std::vector<int> keys;
//     keys.reserve(n);

//     for (int i = 0; i < n; i++) {
//         keys.push_back(std::rand() % 5000);
//     }

//     std::cout << "============================================\n";
//     std::cout << " 3-Way Hashing Comparison on Same Dataset\n";
//     std::cout << "============================================\n";
//     std::cout << "Dataset size: " << n << "\n";

//     runLinearHashingTest(keys);
//     runExtendibleHashingTest(keys);
//     runLarsonHashingTest(keys);

//     return 0;
// }


// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>
// #include <chrono>

// #include "LinearHashTable.h"
// #include "ExtendibleHashTable.h"
// #include "LarsonDynamicHashTable.h"

// struct BenchmarkResult {
//     int attemptedKeys;
//     int uniqueKeys;
//     int buckets;
//     int splits;
//     double loadFactor;
//     int successfulSearches;
//     long long insertTimeUs;
//     long long searchTimeUs;
// };

// BenchmarkResult runLinearHashingTest(const std::vector<int>& keys) {
//     LinearHashTable table(2, 2, 0.75);

//     auto startInsert = std::chrono::high_resolution_clock::now();
//     for (int key : keys) {
//         table.insert(key);
//     }
//     auto endInsert = std::chrono::high_resolution_clock::now();

//     auto startSearch = std::chrono::high_resolution_clock::now();
//     int foundCount = 0;
//     for (int round = 0; round < 100; round++) {
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//     }
//     auto endSearch = std::chrono::high_resolution_clock::now();

//     BenchmarkResult result;
//     result.attemptedKeys = static_cast<int>(keys.size());
//     result.uniqueKeys = table.getKeyCount();
//     result.buckets = table.getBucketCount();
//     result.splits = table.getSplitCount();
//     result.loadFactor = table.getLoadFactor();
//     result.successfulSearches = foundCount;
//     result.insertTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert).count();
//     result.searchTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch).count();

//     return result;
// }

// BenchmarkResult runExtendibleHashingTest(const std::vector<int>& keys) {
//     ExtendibleHashTable table(2);

//     auto startInsert = std::chrono::high_resolution_clock::now();
//     for (int key : keys) {
//         table.insert(key);
//     }
//     auto endInsert = std::chrono::high_resolution_clock::now();

//     auto startSearch = std::chrono::high_resolution_clock::now();
//     int foundCount = 0;
//     for (int round = 0; round < 100; round++) {
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//     }
//     auto endSearch = std::chrono::high_resolution_clock::now();

//     BenchmarkResult result;
//     result.attemptedKeys = static_cast<int>(keys.size());
//     result.uniqueKeys = table.getKeyCount();
//     result.buckets = table.getBucketCount();
//     result.splits = table.getSplitCount();
//     result.loadFactor = table.getLoadFactor();
//     result.successfulSearches = foundCount;
//     result.insertTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert).count();
//     result.searchTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch).count();

//     return result;
// }

// BenchmarkResult runLarsonHashingTest(const std::vector<int>& keys) {
//     LarsonDynamicHashTable table(2, 2, 0.75, 0.25);

//     auto startInsert = std::chrono::high_resolution_clock::now();
//     for (int key : keys) {
//         table.insert(key);
//     }
//     auto endInsert = std::chrono::high_resolution_clock::now();

//     auto startSearch = std::chrono::high_resolution_clock::now();
//     int foundCount = 0;
//     for (int round = 0; round < 100; round++) {
//         for (int key : keys) {
//             if (table.contains(key)) {
//                 foundCount++;
//             }
//         }
//     }
//     auto endSearch = std::chrono::high_resolution_clock::now();

//     BenchmarkResult result;
//     result.attemptedKeys = static_cast<int>(keys.size());
//     result.uniqueKeys = table.getKeyCount();
//     result.buckets = table.getBucketCount();
//     result.splits = table.getSplitCount();
//     result.loadFactor = table.getLoadFactor();
//     result.successfulSearches = foundCount;
//     result.insertTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endInsert - startInsert).count();
//     result.searchTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(endSearch - startSearch).count();

//     return result;
// }

// void printResult(const std::string& name, const BenchmarkResult& result) {
//     std::cout << "\n=== " << name << " ===\n";
//     std::cout << "Inserted keys attempted: " << result.attemptedKeys << "\n";
//     std::cout << "Unique keys stored: " << result.uniqueKeys << "\n";
//     std::cout << "Buckets: " << result.buckets << "\n";
//     std::cout << "Splits: " << result.splits << "\n";
//     std::cout << "Load Factor: " << result.loadFactor << "\n";
//     std::cout << "Successful searches: " << result.successfulSearches << "\n";
//     std::cout << "Insertion time: " << result.insertTimeUs << " us\n";
//     std::cout << "Search time: " << result.searchTimeUs << " us\n";
// }

// std::vector<int> generateRandomKeys(int n, int maxValue) {
//     std::vector<int> keys;
//     keys.reserve(n);

//     for (int i = 0; i < n; i++) {
//         keys.push_back(std::rand() % maxValue);
//     }

//     return keys;
// }

// int main() {
//     std::srand(static_cast<unsigned>(std::time(nullptr)));

//     std::vector<int> sizes = {1000, 5000, 10000};

//     std::cout << "============================================\n";
//     std::cout << " Multi-Size Hashing Comparison\n";
//     std::cout << "============================================\n";

//     for (int n : sizes) {
//         std::vector<int> keys = generateRandomKeys(n, n * 5);

//         std::cout << "\n\n############################################\n";
//         std::cout << "Dataset size: " << n << "\n";
//         std::cout << "############################################\n";

//         BenchmarkResult linearResult = runLinearHashingTest(keys);
//         BenchmarkResult extendibleResult = runExtendibleHashingTest(keys);
//         BenchmarkResult larsonResult = runLarsonHashingTest(keys);

//         printResult("Linear Hashing Results", linearResult);
//         printResult("Extendible Hashing Results", extendibleResult);
//         printResult("Larson Dynamic Hash Table Results", larsonResult);
//     }

//     return 0;
// }

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "LinearHashTable.h"
#include "ExtendibleHashTable.h"
#include "LarsonDynamicHashTable.h"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const int n = 20;
    std::vector<int> keys;

    // Generate random keys
    std::cout << "============================================\n";
    std::cout << " Random Keys Demonstration\n";
    std::cout << "============================================\n";

    std::cout << "\nKeys:\n";
    for (int i = 0; i < n; i++) {
        int key = std::rand() % 100;
        keys.push_back(key);
        std::cout << key << " ";
    }
    std::cout << "\n\n";

    // Create tables
    LinearHashTable linear(2, 2, 0.75);
    ExtendibleHashTable extendible(2);
    LarsonDynamicHashTable larson(2, 2, 0.75, 0.25);

    // Insert keys
    for (int key : keys) {
        std::cout << "Inserting " << key << "\n";
        linear.insert(key);
        extendible.insert(key);
        larson.insert(key);
    }

    // Print results
    linear.print();
    extendible.print();
    larson.print();

    // Simple test (search + delete)
    if (!keys.empty()) {
        int testKey = keys[0];

        std::cout << "\n===== Testing Operations =====\n";

        std::cout << "\nTesting key: " << testKey << "\n";

        std::cout << "Linear Contains: "
                  << (linear.contains(testKey) ? "Yes" : "No") << "\n";

        std::cout << "Extendible Contains: "
                  << (extendible.contains(testKey) ? "Yes" : "No") << "\n";

        std::cout << "Larson Contains: "
                  << (larson.contains(testKey) ? "Yes" : "No") << "\n";

        std::cout << "\nRemoving key: " << testKey << "\n";

        linear.remove(testKey);
        extendible.remove(testKey);
        larson.remove(testKey);

        std::cout << "\nAfter Removal:\n";

        std::cout << "Linear Contains: "
                  << (linear.contains(testKey) ? "Yes" : "No") << "\n";

        std::cout << "Extendible Contains: "
                  << (extendible.contains(testKey) ? "Yes" : "No") << "\n";

        std::cout << "Larson Contains: "
                  << (larson.contains(testKey) ? "Yes" : "No") << "\n";
    }

    return 0;
}

// g++ src/ExtendibleHashTable.cpp src/LarsonDynamicHashTable.cpp src/LinearHashTable.cpp src/main.cpp -o app.exe
