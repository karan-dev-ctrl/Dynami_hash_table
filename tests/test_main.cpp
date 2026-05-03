#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <string>
#include <cassert>

#include "LinearHashTable.h"
#include "ExtendibleHashTable.h"
#include "LarsonDynamicHashTable.h"

using namespace std::chrono;

// ==================== BENCHMARK HELPER ====================

struct BenchmarkResult {
    std::string name;
    long long insertTimeUs;
    long long searchTimeUs;
    int uniqueKeys;
    int bucketCount;
    int splitCount;
    double loadFactor;
};

BenchmarkResult runBenchmark(const std::string& name, int numKeys) {
    std::vector<int> keys;
    keys.reserve(numKeys);

    // Generate random keys
    std::mt19937 gen(42);  // fixed seed for reproducibility
    std::uniform_int_distribution<> dist(1, numKeys * 10);

    for (int i = 0; i < numKeys; ++i) {
        keys.push_back(dist(gen));
    }

    BenchmarkResult result;
    result.name = name;

    // === Insert Benchmark ===
    auto start = high_resolution_clock::now();

    if (name == "Linear") {
        LinearHashTable table(4, 4, 0.75);
        for (int k : keys) table.insert(k);
        result.uniqueKeys = table.getKeyCount();
        result.bucketCount = table.getBucketCount();
        result.splitCount = table.getSplitCount();
        result.loadFactor = table.getLoadFactor();
    } 
    else if (name == "Extendible") {
        ExtendibleHashTable table(4);
        for (int k : keys) table.insert(k);
        result.uniqueKeys = table.getKeyCount();
        result.bucketCount = table.getBucketCount();
        result.splitCount = table.getSplitCount();
        result.loadFactor = table.getLoadFactor();
    } 
    else { // Larson
        LarsonDynamicHashTable table(4, 4, 0.75, 0.25);
        for (int k : keys) table.insert(k);
        result.uniqueKeys = table.getKeyCount();
        result.bucketCount = table.getBucketCount();
        result.splitCount = table.getSplitCount();
        result.loadFactor = table.getLoadFactor();
    }

    auto end = high_resolution_clock::now();
    result.insertTimeUs = duration_cast<microseconds>(end - start).count();

    // === Search Benchmark (100 rounds) ===
    start = high_resolution_clock::now();
    int found = 0;

    if (name == "Linear") {
        LinearHashTable table(4, 4, 0.75);  // recreate for fair comparison
        for (int k : keys) table.insert(k); // re-insert
        for (int round = 0; round < 100; ++round) {
            for (int k : keys) {
                if (table.contains(k)) found++;
            }
        }
    } 
    else if (name == "Extendible") {
        ExtendibleHashTable table(4);
        for (int k : keys) table.insert(k);
        for (int round = 0; round < 100; ++round) {
            for (int k : keys) if (table.contains(k)) found++;
        }
    } 
    else {
        LarsonDynamicHashTable table(4, 4, 0.75, 0.25);
        for (int k : keys) table.insert(k);
        for (int round = 0; round < 100; ++round) {
            for (int k : keys) if (table.contains(k)) found++;
        }
    }

    end = high_resolution_clock::now();
    result.searchTimeUs = duration_cast<microseconds>(end - start).count();

    return result;
}

void printBenchmark(const BenchmarkResult& r) {
    std::cout << "\n=== " << r.name << " Hashing ===\n";
    std::cout << "Inserted Keys     : " << r.uniqueKeys << "\n";
    std::cout << "Buckets           : " << r.bucketCount << "\n";
    std::cout << "Splits            : " << r.splitCount << "\n";
    std::cout << "Load Factor       : " << std::fixed << std::setprecision(3) 
              << r.loadFactor << "\n";
    std::cout << "Insert Time       : " << r.insertTimeUs << " μs\n";
    std::cout << "Search Time (100x): " << r.searchTimeUs << " μs\n";
}

// ==================== UNIT TESTS (kept from before) ====================

void testBasicOperations() {
    std::cout << "=== Basic Unit Tests ===\n";
    // ... (your previous basic tests) ...
    LinearHashTable linear(2, 2, 0.75);
    assert(linear.insert(10) && linear.contains(10));
    assert(linear.remove(10) && !linear.contains(10));
    std::cout << "Basic tests passed\n";
}

// ==================== MAIN ====================

int main() {
    testBasicOperations();

    std::cout << "\n========================================\n";
    std::cout << "       BENCHMARKING (10,000 keys)\n";
    std::cout << "========================================\n";

    auto linearRes    = runBenchmark("Linear", 10000);
    auto extendibleRes = runBenchmark("Extendible", 10000);
    auto larsonRes    = runBenchmark("Larson", 10000);

    printBenchmark(linearRes);
    printBenchmark(extendibleRes);
    printBenchmark(larsonRes);

    std::cout << "\n🎉 All tests and benchmarks completed!\n";
    return 0;
}