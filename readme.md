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
└── readme.md
```

---

## Algorithms

| Algorithm | Growth Strategy | Shrink Support |
|-----------|----------------|----------------|
| **Linear Hashing** | Splits buckets one at a time in order | No |
| **Extendible Hashing** | Splits via directory + global/local depth bits | No |
| **Larson Dynamic Hashing** | Like Linear, but also merges on low load | Yes |

---

## Benchmark Scenario

`main.cpp` runs a **4-round cumulative benchmark** — all three tables are inserted
into without being reset between rounds.

| Round | Keys | Max Value |
|-------|------|-----------|
| 1 | 1,000,000 | 10,000,000 |
| 2 | 1,000,000 | 20,000,000 |
| 3 | 1,000,000 | 30,000,000 |
| 4 | 1,000,000 | UINT32_MAX (4,294,967,295) |

Each round reports per table:
- Unique keys stored, bucket count, split count, load factor
- Memory usage (MB)
- Insert CPU time (ms)
- Point query CPU time (ms)

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
| `initBuckets` | 262144 (2^18) | Starting bucket count |
| `capacity` | 4 | Max keys per bucket before split |
| `maxLoad` | 0.75 | Split trigger threshold |
| `minLoad` | 0.25 | Merge trigger threshold (Larson only) |
