/*
 * Benchmark: 4-round cumulative insertion test
 *
 * Each round inserts 1,000,000 uint32_t keys into all three hash tables
 * (tables are NOT reset between rounds — insertion is cumulative).
 *
 * Per round, per table:
 *   - Insert all keys  → report CPU time + table size
 *   - Query all keys   → report CPU time + hit count
 *
 * Rounds
 *   1: max key value =  10,000,000
 *   2: max key value =  20,000,000
 *   3: max key value =  30,000,000
 *   4: max key value =  UINT32_MAX (4,294,967,295)
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <chrono>
#include <limits>
#include <random>
#include <string>

#include "LinearHashTable.h"
#include "ExtendibleHashTable.h"
#include "LarsonDynamicHashTable.h"

// ── helpers ──────────────────────────────────────────────────────────────────

static std::vector<uint32_t> generateKeys(uint32_t count,
                                          uint32_t maxVal,
                                          std::mt19937& rng) {
    std::uniform_int_distribution<uint32_t> dist(0u, maxVal);
    std::vector<uint32_t> keys(count);
    for (auto& k : keys) k = dist(rng);
    return keys;
}

static void printSeparator(const std::string& title) {
    std::cout << "\n";
    std::cout << "============================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "============================================================\n";
}

static void printTableHeader() {
    std::cout << std::left
              << std::setw(26) << "Metric"
              << std::setw(18) << "Linear"
              << std::setw(20) << "Extendible"
              << std::setw(18) << "Larson"
              << "\n";
    std::cout << std::string(82, '-') << "\n";
}

// ── per-table benchmark run ───────────────────────────────────────────────────

template<typename Table>
struct RoundResult {
    long long uniqueKeys;
    long long bucketCount;
    long long splitCount;
    double    loadFactor;
    double    memoryMB;
    double    pageUtilization;    // % of non-empty buckets
    double    insertMs;           // wall-clock time in milliseconds
    double    insertThroughput;   // million inserts per second
    double    queryMs;            // wall-clock time in milliseconds
    double    queryThroughput;    // million queries per second
    long long hits;
};

template<typename Table>
RoundResult<Table> runRound(Table& table,
                            const std::vector<uint32_t>& keys,
                            const std::string& name) {
    RoundResult<Table> res{};
    const uint32_t total    = static_cast<uint32_t>(keys.size());
    const uint32_t progress = total / 10;   // print every 10 %

    using Clock = std::chrono::high_resolution_clock;
    using Ms    = std::chrono::duration<double, std::milli>;

    // ── insert with progress ──────────────────────────────────────────────
    std::cout << "  " << name << " — inserting..." << std::flush;
    auto t0 = Clock::now();
    for (uint32_t i = 0; i < total; i++) {
        table.insert(keys[i]);
        if (progress > 0 && (i + 1) % progress == 0)
            std::cout << " " << ((i + 1) / progress * 10) << "%" << std::flush;
    }
    auto t1 = Clock::now();
    res.insertMs         = Ms(t1 - t0).count();
    res.insertThroughput = static_cast<double>(total) / (res.insertMs / 1000.0) / 1'000'000.0;
    std::cout << " done.\n" << std::flush;

    // ── snapshot table state ──────────────────────────────────────────────
    res.uniqueKeys       = table.getKeyCount();
    res.bucketCount      = table.getBucketCount();
    res.splitCount       = table.getSplitCount();
    res.loadFactor       = table.getLoadFactor();
    res.memoryMB         = static_cast<double>(table.getMemoryBytes()) / 1024.0 / 1024.0;
    res.pageUtilization  = table.getPageUtilization();

    // ── point query with progress ─────────────────────────────────────────
    long long hits = 0;
    std::cout << "  " << name << " — querying  ..." << std::flush;
    auto q0 = Clock::now();
    for (uint32_t i = 0; i < total; i++) {
        hits += table.contains(keys[i]) ? 1 : 0;
        if (progress > 0 && (i + 1) % progress == 0)
            std::cout << " " << ((i + 1) / progress * 10) << "%" << std::flush;
    }
    auto q1 = Clock::now();
    res.queryMs         = Ms(q1 - q0).count();
    res.queryThroughput = static_cast<double>(total) / (res.queryMs / 1000.0) / 1'000'000.0;
    res.hits            = hits;
    std::cout << " done.\n" << std::flush;

    return res;
}

// ── print one result row ──────────────────────────────────────────────────────

template<typename T>
static void printRow(const std::string& label,
                     T vLinear, T vExtend, T vLarson,
                     int width = 18) {
    std::cout << std::left  << std::setw(26) << label
              << std::left  << std::setw(width) << vLinear
              << std::left  << std::setw(width + 2) << vExtend
              << std::left  << std::setw(width) << vLarson
              << "\n";
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {
    /*
     * Initial configuration for ~1,000,000 items:
     *   initBuckets = 262144 (2^18) with capacity=4 → 1,048,576 initial slots
     *   First split triggers when load exceeds 0.75 (≈ 786K keys inserted)
     */
    LinearHashTable        linear    (262144, 4, 0.75);
    ExtendibleHashTable    extendible(4);
    LarsonDynamicHashTable larson    (262144, 4, 0.75, 0.25);

    struct Round {
        uint32_t    count;
        uint32_t    maxVal;
        const char* label;
    };

    const Round rounds[] = {
        { 1'000'000,  10'000'000u,                          "Round 1 — 1M keys, max value = 10,000,000"    },
        { 1'000'000,  20'000'000u,                          "Round 2 — 1M keys, max value = 20,000,000"    },
        { 1'000'000,  30'000'000u,                          "Round 3 — 1M keys, max value = 30,000,000"    },
        { 1'000'000,  std::numeric_limits<uint32_t>::max(), "Round 4 — 1M keys, max value = UINT32_MAX"    },
    };

    // Fixed seed → reproducible results across runs
    std::mt19937 rng(42u);

    std::cout << "\n";
    std::cout << "************************************************************\n";
    std::cout << "*   Dynamic Hash Table — 4-Round Benchmark                 *\n";
    std::cout << "*   Tables are cumulative (not reset between rounds)       *\n";
    std::cout << "*   Time measured via std::chrono::high_resolution_clock   *\n";
    std::cout << "************************************************************\n";

    for (const auto& round : rounds) {
        // Generate this round's key collection
        std::vector<uint32_t> keys = generateKeys(round.count, round.maxVal, rng);

        printSeparator(round.label);

        // Run all three tables
        std::cout << "\n";
        auto rLinear = runRound(linear,     keys, "Linear    ");
        auto rExtend = runRound(extendible, keys, "Extendible");
        auto rLarson = runRound(larson,     keys, "Larson    ");

        // ── Insert results ──────────────────────────────────────────────
        std::cout << "\n  [INSERT — " << round.count / 1'000'000 << "M keys]\n\n";
        printTableHeader();

        printRow("Unique keys stored",
                 rLinear.uniqueKeys, rExtend.uniqueKeys, rLarson.uniqueKeys);

        printRow("Bucket count",
                 rLinear.bucketCount, rExtend.bucketCount, rLarson.bucketCount);

        printRow("Split count",
                 rLinear.splitCount, rExtend.splitCount, rLarson.splitCount);

        std::cout << std::fixed << std::setprecision(4);
        printRow("Load factor",
                 rLinear.loadFactor, rExtend.loadFactor, rLarson.loadFactor);

        std::cout << std::fixed << std::setprecision(2);
        printRow("Memory (MB)",
                 rLinear.memoryMB, rExtend.memoryMB, rLarson.memoryMB);

        std::cout << std::fixed << std::setprecision(2);
        printRow("Page utilization(%)",
                 rLinear.pageUtilization, rExtend.pageUtilization, rLarson.pageUtilization);

        std::cout << std::fixed << std::setprecision(3);
        printRow("Insert time (ms)",
                 rLinear.insertMs, rExtend.insertMs, rLarson.insertMs);

        std::cout << std::fixed << std::setprecision(3);
        printRow("Insert throughput(Mop/s)",
                 rLinear.insertThroughput, rExtend.insertThroughput, rLarson.insertThroughput);

        // ── Query results ───────────────────────────────────────────────
        std::cout << "\n  [POINT QUERY — " << round.count / 1'000'000 << "M lookups]\n\n";
        printTableHeader();

        printRow("Query hits",
                 rLinear.hits, rExtend.hits, rLarson.hits);

        std::cout << std::fixed << std::setprecision(3);
        printRow("Query time (ms)",
                 rLinear.queryMs, rExtend.queryMs, rLarson.queryMs);

        std::cout << std::fixed << std::setprecision(3);
        printRow("Query throughput(Mop/s)",
                 rLinear.queryThroughput, rExtend.queryThroughput, rLarson.queryThroughput);
    }

    // ── Cumulative totals after all 4 rounds ─────────────────────────────
    printSeparator("Final state after all 4 rounds (4M keys attempted)");
    printTableHeader();

    std::cout << std::defaultfloat;
    printRow("Total keys stored",
             linear.getKeyCount(),     extendible.getKeyCount(),     larson.getKeyCount());
    printRow("Total buckets",
             linear.getBucketCount(),  extendible.getBucketCount(),  larson.getBucketCount());
    printRow("Total splits",
             linear.getSplitCount(),   extendible.getSplitCount(),   larson.getSplitCount());

    std::cout << std::fixed << std::setprecision(4);
    printRow("Load factor",
             linear.getLoadFactor(),   extendible.getLoadFactor(),   larson.getLoadFactor());

    std::cout << std::fixed << std::setprecision(2);
    printRow("Memory (MB)",
             linear.getMemoryBytes()    / 1024.0 / 1024.0,
             extendible.getMemoryBytes()/ 1024.0 / 1024.0,
             larson.getMemoryBytes()    / 1024.0 / 1024.0);

    std::cout << "\n";
    return 0;
}
