# Dynamic Hash Tables — C++ Implementation

A C++17 implementation and benchmark of three classic dynamic hashing algorithms:
**Linear Hashing**, **Extendible Hashing**, and **Larson Dynamic Hashing**.

---

## Folder Structure

```
├── include/
│   ├── LinearHashTable.h           # Linear Hashing header
│   ├── ExtendibleHashTable.h       # Extendible Hashing header
│   └── LarsonDynamicHashTable.h    # Larson Dynamic Hashing header
│
├── src/
│   ├── LinearHashTable.cpp         # Linear Hashing implementation
│   ├── ExtendibleHashTable.cpp     # Extendible Hashing implementation
│   └── LarsonDynamicHashTable.cpp  # Larson Dynamic Hashing implementation
│
├── tests/
│   └── test_main.cpp               # Correctness tests
│
├── main.cpp                        # 4-round benchmark (entry point)
├── CMakeLists.txt                  # CMake build config
├── OUTPUT_ANALYSIS.md              # Full benchmark report and analysis
└── readme.md
```

---

## Algorithms

| Algorithm | Growth Strategy | Shrink Support |
|-----------|----------------|----------------|
| **Linear Hashing** | Splits buckets one at a time in fixed order | No |
| **Extendible Hashing** | Splits via directory + global/local depth bits | No |
| **Larson Dynamic Hashing** | Like Linear, but also merges buckets on low load | Yes |

---

## Key type

All hash tables use `uint32_t` keys (32-bit unsigned integer, max value 4,294,967,295).

---

## Metrics Reported

Each round reports the following per table:

| Metric | Description |
|--------|-------------|
| Unique keys stored | Total distinct keys in the table |
| Page count | Total number of buckets |
| Page capacity | Maximum keys a single bucket can hold |
| Split count | Total number of bucket splits performed |
| Load factor | keys / (pages × capacity) |
| Memory (MB) | Estimated memory usage |
| Page utilization (%) | % of buckets that contain at least 1 key |
| Insert time (ms) | Wall-clock time to insert the round's keys |
| Insert throughput (Mop/s) | Million insert operations per second |
| Query hits | Number of successful point queries |
| Query time (ms) | Wall-clock time to query all inserted keys |
| Query throughput (Mop/s) | Million query operations per second |

> Time is measured using `std::chrono::high_resolution_clock` (nanosecond precision).

---

## Benchmark Scenario

`main.cpp` runs a **4-round cumulative benchmark** — all three tables are inserted
into without being reset between rounds.

| Round | Keys | Max Value | Purpose |
|-------|------|-----------|---------|
| 1 | 1,000,000 | 10,000,000 | Dense range — high duplicate probability |
| 2 | 1,000,000 | 20,000,000 | Medium range |
| 3 | 1,000,000 | 30,000,000 | Wider range |
| 4 | 1,000,000 | UINT32_MAX (4,294,967,295) | Full uint32 space — minimal duplicates |

---

## Sample Output

```
  [INSERT — 1M keys]

Metric                    Linear            Extendible          Larson
---------------------------------------------------------------------------
Unique keys stored        951464            951464              951464
Page count                317155            337998              317155
Page capacity             4                 4                   4
Split count               55011             337996              55011
Load factor               0.7500            0.7037              0.7500
Memory (MB)               7.26              40.79               7.26
Page utilization(%)       93.51             98.65               93.51
Insert time (ms)          360.153           1018.180            326.446
Insert throughput(Mop/s)  2.777             0.982               3.063

  [POINT QUERY — 1M lookups]

Metric                    Linear            Extendible          Larson
---------------------------------------------------------------------------
Query hits                1000000           1000000             1000000
Query time (ms)           100.365           128.535             96.168
Query throughput(Mop/s)   9.964             7.780               10.398
```

---

## How to Build and Run

### Option 1 — Direct g++ (quickest)

```bash
# Build benchmark
g++ -std=c++17 -O2 -Wall -Iinclude src/LinearHashTable.cpp src/ExtendibleHashTable.cpp src/LarsonDynamicHashTable.cpp main.cpp -o benchmark.exe

# Run
.\benchmark.exe       # Windows
./benchmark.exe       # Linux / Mac
```

```bash
# Build and run tests
g++ -std=c++17 -O2 -Wall -Iinclude src/LinearHashTable.cpp src/ExtendibleHashTable.cpp src/LarsonDynamicHashTable.cpp tests/test_main.cpp -o test.exe

.\test.exe
```

### Option 2 — CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .

.\App.exe      # Windows
./App          # Linux / Mac
```

---

## Requirements

- C++17 or later
- g++ / clang++ / MSVC
- CMake 3.16+ (only if using Option 2)

---

## Initial Table Configuration

All three tables are pre-configured for ~1 million items:

```cpp
LinearHashTable        linear    (262144, 4, 0.75);
ExtendibleHashTable    extendible(4);
LarsonDynamicHashTable larson    (262144, 4, 0.75, 0.25);
```

| Parameter | Value | Meaning |
|-----------|-------|---------|
| `initBuckets` | 262144 (2^18) | Starting bucket count for Linear & Larson |
| `capacity` | 4 | Max keys per bucket before split triggers |
| `maxLoad` | 0.75 | Split threshold |
| `minLoad` | 0.25 | Merge threshold (Larson only) |

---

## Available Methods (all 3 tables)

```cpp
bool   insert(uint32_t key);
bool   contains(uint32_t key) const;
bool   remove(uint32_t key);

int    getKeyCount()        const;  // total unique keys stored
int    getBucketCount()     const;  // total pages/buckets
int    getSplitCount()      const;  // total splits performed
int    getPageCapacity()    const;  // max keys per bucket
double getLoadFactor()      const;  // keys / (buckets x capacity)
size_t getMemoryBytes()     const;  // estimated memory usage
double getPageUtilization() const;  // % non-empty buckets
```

---

## Documentation

See [OUTPUT_ANALYSIS.md](OUTPUT_ANALYSIS.md) for the full benchmark report including:
- Detailed description of each algorithm
- All 4-round results with throughput
- Performance comparison across all metrics
- Conclusions and recommendations
