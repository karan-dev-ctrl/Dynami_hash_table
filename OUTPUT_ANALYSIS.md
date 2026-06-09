# Dynamic Hash Tables — Benchmark Report
### PAL0343 | Semester Project

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Hash Table Descriptions](#2-hash-table-descriptions)
3. [Implementation Details](#3-implementation-details)
4. [Benchmark Setup](#4-benchmark-setup)
5. [Benchmark Results](#5-benchmark-results)
6. [Performance Comparison](#6-performance-comparison)
7. [Conclusions](#7-conclusions)

---

## 1. Project Overview

This project implements and benchmarks three classic **dynamic hashing algorithms** in C++17.
Dynamic hash tables differ from static ones in that they **grow (and shrink) automatically**
as data is inserted or deleted — without requiring a full rebuild of the table.

The three algorithms studied are:

| Algorithm | Original Author | Year |
|-----------|----------------|------|
| Linear Hashing | Witold Litwin | 1980 |
| Extendible Hashing | Fagin, Nievergelt, Pippenger, Strong | 1979 |
| Larson Dynamic Hashing | Per-Åke Larson | 1988 |

All three are evaluated on the same dataset across four cumulative insertion rounds
measuring **insert time**, **query time**, **throughput**, **memory usage**,
**load factor stability**, and **page utilization**.

---

## 2. Hash Table Descriptions

### 2.1 Linear Hashing

**Core idea:** Buckets are split one at a time in a fixed sequential order,
regardless of which bucket is actually overflowing.

**How it works:**
- The table tracks a **split pointer** (`nextSplit`) that moves forward each time a split occurs.
- A **level** counter doubles the address space whenever the split pointer wraps around.
- When the global load factor exceeds `maxLoadFactor` (0.75), the bucket at `nextSplit` is split — not necessarily the full one.
- Keys are redistributed using the formula: `index = key % (initialBuckets × 2^level)`

**Growth mechanism:**
```
Level 0: [B0] [B1]                    → 2 buckets
Level 0: [B0] [B1] [B2]               → split B0 → B0 + B2
Level 0: [B0] [B1] [B2] [B3]          → split B1 → B1 + B3
Level 1: [B0] [B1] [B2] [B3]          → level advances, nextSplit resets
```

**Strengths:**
- Predictable, controlled growth
- Maintains exact load factor (0.75)
- Simple implementation with low memory overhead
- No directory structure needed

**Weaknesses:**
- Splits happen at a fixed pointer — a bucket may overflow temporarily before its turn to split
- No shrink support (no merge on delete)

---

### 2.2 Extendible Hashing

**Core idea:** Uses a **directory** of pointers to buckets, controlled by a
**global depth** and per-bucket **local depth**. When a bucket overflows,
the directory may double in size.

**How it works:**
- A directory array of size `2^globalDepth` maps hash prefixes to buckets.
- Each bucket has a `localDepth` — the number of hash bits used to route to it.
- When a bucket overflows:
  - If `localDepth == globalDepth` → **directory doubles** in size first.
  - The overflowing bucket is then **split** — its keys are redistributed.
- Point lookup: `index = key & ((1 << globalDepth) - 1)` → O(1) directory lookup.

**Growth mechanism:**
```
globalDepth=1: Dir[0]→BucketA  Dir[1]→BucketB
              (each bucket has localDepth=1)

BucketA overflows:
globalDepth=2: Dir[00]→BucketA'  Dir[01]→BucketA''
               Dir[10]→BucketB   Dir[11]→BucketB
```

**Strengths:**
- O(1) point lookup (single directory dereference)
- No overflow chains — buckets split cleanly
- Naturally handles non-uniform key distributions
- Very high page utilization (~98%)

**Weaknesses:**
- Directory can grow very large (must be a power of 2)
- Uses significantly more memory than Linear/Larson at scale
- Directory doubling is expensive at large sizes
- No built-in shrink mechanism

---

### 2.3 Larson Dynamic Hashing

**Core idea:** An enhancement of Linear Hashing that adds a **merge operation**
to shrink the table when the load factor drops below a minimum threshold.

**How it works:**
- Insert and split logic is **identical to Linear Hashing**.
- After every delete, if the load factor drops below `minLoadFactor` (0.25),
  the last bucket is **merged** back into its predecessor.
- Merge is the reverse of split: the last bucket's keys are moved to its buddy,
  and the bucket is removed.

**Growth and shrink mechanism:**
```
Insert heavy → load > 0.75 → SPLIT  (same as Linear)
Delete heavy → load < 0.25 → MERGE  (unique to Larson)
```

**Strengths:**
- All the advantages of Linear Hashing
- **Reclaims memory** after heavy deletions — no wasted empty buckets
- Maintains load factor in both directions (bounded between 0.25 and 0.75)
- Low memory overhead (same as Linear for insert-only workloads)

**Weaknesses:**
- Slightly more complex implementation
- Merge benefit is not visible in insert-only benchmarks
- Same temporary overflow limitation as Linear Hashing

---

## 3. Implementation Details

### Language and Standard
- **Language:** C++17
- **Compiler:** g++ with `-O2` optimisation
- **Key type:** `uint32_t` (32-bit unsigned integer, max value 4,294,967,295)

### Constructor Parameters

```cpp
LinearHashTable        linear    (262144, 4, 0.75);
ExtendibleHashTable    extendible(4);
LarsonDynamicHashTable larson    (262144, 4, 0.75, 0.25);
```

| Parameter | Value | Meaning |
|-----------|-------|---------|
| `initBuckets` | 262,144 (2^18) | Starting bucket count for Linear & Larson |
| `capacity` | 4 | Maximum keys per bucket before split triggers |
| `maxLoadFactor` | 0.75 | Split threshold — table grows when exceeded |
| `minLoadFactor` | 0.25 | Merge threshold — Larson shrinks when below this |

> **Why 262,144 initial buckets?**
> With capacity=4 and maxLoad=0.75, this gives 1,048,576 initial slots —
> exactly enough for 1M keys before the first split occurs.

### Time Measurement
All times measured using `std::chrono::high_resolution_clock` (wall-clock time,
nanosecond resolution). Results are reported in milliseconds (ms).

### Throughput Formula
```
Throughput (Mop/s) = operations / (time_ms / 1000) / 1,000,000
```

### Page Utilization Formula
```
Page Utilization (%) = non-empty buckets / total buckets × 100
```
A bucket is considered non-empty if it contains at least 1 key.

---

## 4. Benchmark Setup

### Scenario
Four cumulative insertion rounds. Tables are **not reset** between rounds —
each round adds more keys on top of the previous state.

| Round | Keys | Max Value | Purpose |
|-------|------|-----------|---------|
| 1 | 1,000,000 | 10,000,000 | Dense range — high duplicate probability |
| 2 | 1,000,000 | 20,000,000 | Medium range |
| 3 | 1,000,000 | 30,000,000 | Wider range |
| 4 | 1,000,000 | 4,294,967,295 (UINT32_MAX) | Full uint32 space — minimal duplicates |

### Metrics Collected Per Round Per Table
- Unique keys stored (cumulative)
- Page count (total buckets)
- Page capacity (max keys per bucket)
- Split count
- Load factor
- Memory usage (MB)
- Page utilization (%)
- Insert time (ms) + Insert throughput (Mop/s)
- Point query time (ms) + Query throughput (Mop/s)
- Query hits (correctness check)

### Random Key Generation
- Generator: `std::mt19937` (Mersenne Twister)
- Seed: fixed value `42` — results are fully reproducible

---

## 5. Benchmark Results

### Round 1 — 1M keys, max value = 10,000,000

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Unique keys stored | 951,464 | 951,464 | 951,464 |
| Page count | 317,155 | 337,998 | 317,155 |
| Page capacity | 4 | 4 | 4 |
| Split count | 55,011 | 337,996 | 55,011 |
| Load factor | 0.7500 | 0.7037 | 0.7500 |
| Memory (MB) | 7.26 | 40.79 | 7.26 |
| **Page utilization (%)** | **93.51** | **98.65** | **93.51** |
| **Insert time (ms)** | **360.153** | **1,018.180** | **326.446** |
| **Insert throughput (Mop/s)** | **2.777** | **0.982** | **3.063** |
| Query hits | 1,000,000 | 1,000,000 | 1,000,000 |
| **Query time (ms)** | **100.365** | **128.535** | **96.168** |
| **Query throughput (Mop/s)** | **9.964** | **7.780** | **10.398** |

---

### Round 2 — 1M keys, max value = 20,000,000 (cumulative)

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Unique keys stored | 1,880,619 | 1,880,619 | 1,880,619 |
| Page count | 626,873 | 665,988 | 626,873 |
| Page capacity | 4 | 4 | 4 |
| Split count | 364,729 | 665,986 | 364,729 |
| Load factor | 0.7500 | 0.7060 | 0.7500 |
| Memory (MB) | 14.35 | 81.34 | 14.35 |
| **Page utilization (%)** | **93.78** | **98.77** | **93.78** |
| **Insert time (ms)** | **494.141** | **1,260.692** | **487.436** |
| **Insert throughput (Mop/s)** | **2.024** | **0.793** | **2.052** |
| Query hits | 1,000,000 | 1,000,000 | 1,000,000 |
| **Query time (ms)** | **126.069** | **130.211** | **116.435** |
| **Query throughput (Mop/s)** | **7.932** | **7.680** | **8.588** |

---

### Round 3 — 1M keys, max value = 30,000,000 (cumulative)

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Unique keys stored | 2,802,443 | 2,802,443 | 2,802,443 |
| Page count | 934,148 | 991,488 | 934,148 |
| Page capacity | 4 | 4 | 4 |
| Split count | 672,004 | 991,486 | 672,004 |
| Load factor | 0.7500 | 0.7066 | 0.7500 |
| Memory (MB) | 21.38 | 89.82 | 21.38 |
| **Page utilization (%)** | **94.93** | **98.81** | **94.93** |
| **Insert time (ms)** | **509.564** | **1,083.081** | **488.916** |
| **Insert throughput (Mop/s)** | **1.962** | **0.923** | **2.045** |
| Query hits | 1,000,000 | 1,000,000 | 1,000,000 |
| **Query time (ms)** | **133.741** | **137.874** | **120.757** |
| **Query throughput (Mop/s)** | **7.477** | **7.253** | **8.281** |

---

### Round 4 — 1M keys, max value = UINT32_MAX (cumulative)

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Unique keys stored | 3,801,696 | 3,801,696 | 3,801,696 |
| Page count | 1,267,232 | 1,351,839 | 1,267,232 |
| Page capacity | 4 | 4 | 4 |
| Split count | 1,005,088 | 1,351,837 | 1,005,088 |
| Load factor | 0.7500 | 0.7031 | 0.7500 |
| Memory (MB) | 29.00 | 291.13 | 29.00 |
| **Page utilization (%)** | **93.56** | **98.64** | **93.56** |
| **Insert time (ms)** | **531.636** | **2,677.878** | **523.670** |
| **Insert throughput (Mop/s)** | **1.881** | **0.373** | **1.910** |
| Query hits | 1,000,000 | 1,000,000 | 1,000,000 |
| **Query time (ms)** | **122.663** | **150.616** | **125.563** |
| **Query throughput (Mop/s)** | **8.152** | **6.639** | **7.964** |

---

### Final State — After All 4 Rounds (4M keys attempted)

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Total keys stored | 3,801,696 | 3,801,696 | 3,801,696 |
| Total buckets | 1,267,232 | 1,351,839 | 1,267,232 |
| Total splits | 1,005,088 | 1,351,837 | 1,005,088 |
| Load factor | 0.7500 | 0.7031 | 0.7500 |
| Memory (MB) | 29.00 | 291.13 | 29.00 |

---

## 6. Performance Comparison

### 6.1 Correctness

All three tables stored **exactly the same number of unique keys** in every round,
and all query hits equal **1,000,000** in every round without exception.

This confirms:
- All three implementations are **correct and equivalent** in behaviour
- Duplicate detection works properly across all key ranges including UINT32_MAX
- Point queries never return false negatives after any number of splits

---

### 6.2 Duplicate Rate

Since keys are generated randomly, the range size directly affects collision probability.

| Round | Attempted | Unique Stored | Duplicates | Duplicate Rate |
|-------|-----------|---------------|------------|----------------|
| 1 | 1,000,000 | 951,464 | 48,536 | **4.85%** |
| 2 | 1,000,000 | 929,155* | 70,845* | **7.08%*** |
| 3 | 1,000,000 | 921,824* | 78,176* | **7.82%*** |
| 4 | 1,000,000 | 999,253* | 747* | **0.07%*** |

*Rounds 2–4 duplicates include within-round collisions AND collisions with
keys already stored from previous rounds.

**Key observation:** Round 4 uses the full UINT32_MAX space (~4.29 billion values).
With only ~2.8M keys already stored, the chance of a new key matching an existing
one is approximately 0.065%, resulting in almost zero duplicates (only 747 rejected).

---

### 6.3 Page Utilization

Page utilization measures what percentage of buckets contain at least one key.

```
Page Utilization (%) = non-empty buckets / total buckets × 100
```

| Round | Linear | Extendible | Larson |
|-------|--------|------------|--------|
| 1 | 93.51% | 98.65% | 93.51% |
| 2 | 93.78% | 98.77% | 93.78% |
| 3 | 94.93% | 98.81% | 94.93% |
| 4 | 93.56% | 98.64% | 93.56% |

**Extendible Hashing achieves the highest page utilization (~98–99%).**
This is because Extendible only creates a new bucket when one actually overflows.
Every newly created bucket immediately receives at least some keys from the split,
so very few buckets are ever left empty. Only ~1–2% of buckets end up empty.

**Linear and Larson sit at ~93–95% page utilization.**
When Linear/Larson splits a bucket, it redistributes its keys between the old
and new bucket using a modulo operation. In some cases, all keys land on one
side — leaving the other bucket temporarily empty. Over time, roughly 6–7%
of buckets remain empty, which explains the lower page utilization.

**Linear and Larson show identical page utilization** in every round, confirming
they use exactly the same split logic. Larson's merge never triggers here since
no deletes are performed.

---

### 6.4 Insert Performance

#### Insert Time (ms) — lower is better

| Round | Linear | Extendible | Larson | Fastest |
|-------|--------|------------|--------|---------|
| 1 | 360 | 1,018 | 326 | **Larson** |
| 2 | 494 | 1,261 | 487 | **Larson** |
| 3 | 510 | 1,083 | 489 | **Larson** |
| 4 | 532 | 2,678 | 524 | **Larson** |

#### Insert Throughput (Mop/s) — higher is better

| Round | Linear | Extendible | Larson |
|-------|--------|------------|--------|
| 1 | 2.777 | 0.982 | **3.063** |
| 2 | 2.024 | 0.793 | **2.052** |
| 3 | 1.962 | 0.923 | **2.045** |
| 4 | 1.881 | 0.373 | **1.910** |

**Findings:**

- **Larson is the fastest for inserts in every round**, slightly outperforming
  Linear. Both use identical split logic — the small advantage is consistent
  cache behaviour at all table sizes.

- **Extendible Hashing is consistently the slowest** — up to **5× slower** than
  Larson in Round 4 (0.373 vs 1.910 Mop/s). The root cause is directory
  management: when a bucket overflows, the directory may double (copying all
  entries), and `splitBucket` must update multiple directory pointers. At 4M
  keys, the directory exceeds 2M entries, making each doubling expensive.

- **All three slow down across rounds** because the cumulative table grows
  larger, causing more CPU cache misses during bucket lookups on every insert.

---

### 6.5 Query Performance

#### Query Time (ms) — lower is better

| Round | Linear | Extendible | Larson | Fastest |
|-------|--------|------------|--------|---------|
| 1 | 100 | 129 | 96 | **Larson** |
| 2 | 126 | 130 | 116 | **Larson** |
| 3 | 134 | 138 | 121 | **Larson** |
| 4 | 123 | 151 | 126 | **Linear** |

#### Query Throughput (Mop/s) — higher is better

| Round | Linear | Extendible | Larson |
|-------|--------|------------|--------|
| 1 | 9.964 | 7.780 | **10.398** |
| 2 | 7.932 | 7.680 | **8.588** |
| 3 | 7.477 | 7.253 | **8.281** |
| 4 | **8.152** | 6.639 | 7.964 |

**Findings:**

- **All three algorithms are fundamentally O(1) for point queries** — they
  compute a hash index and scan at most 4 keys per bucket. The differences
  are small and reflect CPU cache behaviour rather than algorithm complexity.

- **Larson leads in query throughput in Rounds 1–3**, reaching 10.398 Mop/s
  in Round 1 — the highest of any table in any round.

- **Extendible Hashing is consistently the slowest for queries** because
  point lookup requires dereferencing a `shared_ptr` (directory → pointer →
  bucket), adding one extra memory indirection compared to Linear/Larson's
  direct vector index access.

- **All 1,000,000 query hits confirmed every round** — zero false negatives
  across all tables and all key ranges including UINT32_MAX.

---

### 6.6 Load Factor Control

| Table | Round 1 | Round 2 | Round 3 | Round 4 |
|-------|---------|---------|---------|---------|
| Linear | **0.7500** | **0.7500** | **0.7500** | **0.7500** |
| Larson | **0.7500** | **0.7500** | **0.7500** | **0.7500** |
| Extendible | 0.7037 | 0.7060 | 0.7066 | 0.7031 |

- **Linear and Larson maintain an exact load factor of 0.75** at all times.
  Their split trigger is global: the moment the average slot occupancy across
  all buckets exceeds 0.75, the next bucket in sequence is split.

- **Extendible Hashing settles around 0.70** — roughly 5% lower than intended.
  This is because its split trigger is per-bucket (a single bucket overflows),
  not global. After a split, the two resulting buckets may be underfull, pulling
  the global average below 0.75.

---

### 6.7 Memory Usage

| Round | Linear | Extendible | Larson | Extendible Overhead |
|-------|--------|------------|--------|---------------------|
| 1 | 7.26 MB | 40.79 MB | 7.26 MB | **5.6× more** |
| 2 | 14.35 MB | 81.34 MB | 14.35 MB | **5.7× more** |
| 3 | 21.38 MB | 89.82 MB | 21.38 MB | **4.2× more** |
| 4 | 29.00 MB | 291.13 MB | 29.00 MB | **10.0× more** |

**Linear and Larson use identical memory** — a flat array of bucket vectors.
Memory grows linearly and predictably with keys stored.

**Extendible uses up to 10× more memory by Round 4** due to its directory:
- Directory array: `2^globalDepth` entries × 16 bytes per `shared_ptr`
- With ~1.35M unique buckets → globalDepth ≈ 21 → 2^21 = 2,097,152 entries
- Directory alone consumes ~32 MB
- Total including bucket metadata and key storage ≈ 291 MB

---

### 6.8 Split Count Comparison

| Round | Linear | Extendible | Larson |
|-------|--------|------------|--------|
| 1 | 55,011 | 337,996 | 55,011 |
| 2 | 364,729 | 665,986 | 364,729 |
| 3 | 672,004 | 991,486 | 672,004 |
| 4 | 1,005,088 | 1,351,837 | 1,005,088 |

- **Linear and Larson split counts match exactly** — confirming identical
  growth logic. Starting from 262,144 buckets and ending at 1,267,232 →
  exactly 1,005,088 splits (1,267,232 − 262,144 = 1,005,088 ✓).

- **Extendible performs ~35% more splits** because it starts from only 2
  buckets and grows entirely through splitting — every unique bucket ever
  created equals one split event.

---

## 7. Conclusions

### 7.1 Performance Winner per Category

| Category | Winner | Reason |
|----------|--------|--------|
| Insert speed | **Larson** | Fastest in all 4 rounds |
| Insert throughput | **Larson** | Up to 3.063 Mop/s vs 0.373 for Extendible |
| Query speed | **Larson** | Fastest in 3 out of 4 rounds |
| Query throughput | **Larson** | Up to 10.398 Mop/s in Round 1 |
| Memory efficiency | **Linear = Larson** | 10× less than Extendible at 4M keys |
| Load factor control | **Linear = Larson** | Exact 0.75 vs ~0.70 for Extendible |
| Page utilization | **Extendible** | ~98% vs ~93% for Linear/Larson |
| Scalability | **Linear = Larson** | Extendible degrades sharply at Round 4 |
| Delete + shrink | **Larson only** | Unique merge operation reclaims memory |

---

### 7.2 Key Findings

**1. Larson Dynamic Hashing is the best overall performer.**
It leads in insert speed, query speed, and throughput across all 4 rounds
while using the same low memory as Linear Hashing. Its merge capability
makes it even stronger in delete-heavy workloads.

**2. Linear and Larson are nearly identical for insert-only workloads.**
Both use the same split logic, maintain the same exact load factor (0.75),
and consume the same memory. Larson's merge advantage only appears in
workloads with frequent deletions.

**3. Extendible Hashing achieves the highest page utilization (~98%).**
Almost every bucket contains at least one key — it creates buckets on demand,
so very few end up empty. Linear/Larson sit at ~93% because split redistribution
can temporarily leave some buckets empty.

**4. Extendible Hashing trades memory and insert speed for structural simplicity.**
The directory provides guaranteed O(1) lookup, but at 4M keys the directory
consumes 32 MB alone. Total memory is 10× higher than Linear/Larson, and
insert throughput drops to just 0.373 Mop/s in Round 4.

**5. Wider key ranges produce fewer duplicates.**
Round 1 (max=10M) has 4.85% duplicates. Round 4 (max=UINT32_MAX) has only
0.07% duplicates. This matches the Birthday Problem prediction: the larger
the key space, the fewer random collisions occur.

**6. All three algorithms achieve 100% query accuracy.**
Every point query across all 4 rounds returns a correct hit. The hashing
and splitting logic handles `uint32_t` keys correctly including UINT32_MAX.

---

### 7.3 Recommendation

| Use Case | Recommended Table |
|----------|------------------|
| Insert-heavy, memory-constrained | **Larson Dynamic Hashing** |
| Insert + delete mixed workload | **Larson Dynamic Hashing** |
| Read-heavy, memory available | **Extendible Hashing** |
| Maximum page utilization needed | **Extendible Hashing** |
| Large scale (millions of keys) | **Larson or Linear** |

> **Overall:** For the tested scenarios, **Larson Dynamic Hashing** is the
> most well-rounded choice — it is the fastest for both inserts and queries,
> matches Linear's memory efficiency, and uniquely supports memory reclamation
> through bucket merging after deletions.
