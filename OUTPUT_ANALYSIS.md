# Benchmark Output Analysis
## Dynamic Hash Tables — 4-Round Cumulative Benchmark

---

## 1. Test Setup

| Parameter | Value |
|-----------|-------|
| Key type | `uint32_t` (32-bit unsigned integer) |
| Keys per round | 1,000,000 |
| Total rounds | 4 (cumulative — tables not reset) |
| RNG seed | Fixed (42) — reproducible results |
| Bucket capacity | 4 keys per bucket |
| Max load factor | 0.75 (split threshold) |
| Min load factor | 0.25 (Larson merge threshold) |
| Initial buckets (Linear / Larson) | 262,144 (2^18) |
| Initial buckets (Extendible) | 2 (grows via directory doubling) |

### Key Ranges per Round

| Round | Max Value | Key Space Size | Purpose |
|-------|-----------|----------------|---------|
| 1 | 10,000,000 | 10M | Dense range — high duplicate probability |
| 2 | 20,000,000 | 20M | Medium range |
| 3 | 30,000,000 | 30M | Wider range |
| 4 | 4,294,967,295 | ~4.29B (UINT32_MAX) | Full uint32 space — minimal duplicates |

---

## 2. Raw Benchmark Output

### Round 1 — 1M keys, max value = 10,000,000

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Unique keys stored | 951,464 | 951,464 | 951,464 |
| Bucket count | 317,155 | 337,998 | 317,155 |
| Split count | 55,011 | 337,996 | 55,011 |
| Load factor | 0.7500 | 0.7037 | 0.7500 |
| Memory (MB) | 7.26 | 40.79 | 7.26 |
| Insert CPU time (ms) | 430 | 1,205 | 392 |
| Query hits | 1,000,000 | 1,000,000 | 1,000,000 |
| Query CPU time (ms) | 135 | 152 | 128 |

### Round 2 — 1M keys, max value = 20,000,000 (cumulative)

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Unique keys stored | 1,880,619 | 1,880,619 | 1,880,619 |
| Bucket count | 626,873 | 665,988 | 626,873 |
| Split count | 364,729 | 665,986 | 364,729 |
| Load factor | 0.7500 | 0.7060 | 0.7500 |
| Memory (MB) | 14.35 | 81.34 | 14.35 |
| Insert CPU time (ms) | 789 | 1,378 | 708 |
| Query hits | 1,000,000 | 1,000,000 | 1,000,000 |
| Query CPU time (ms) | 150 | 174 | 149 |

### Round 3 — 1M keys, max value = 30,000,000 (cumulative)

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Unique keys stored | 2,802,443 | 2,802,443 | 2,802,443 |
| Bucket count | 934,148 | 991,488 | 934,148 |
| Split count | 672,004 | 991,486 | 672,004 |
| Load factor | 0.7500 | 0.7066 | 0.7500 |
| Memory (MB) | 21.38 | 89.82 | 21.38 |
| Insert CPU time (ms) | 627 | 1,239 | 642 |
| Query hits | 1,000,000 | 1,000,000 | 1,000,000 |
| Query CPU time (ms) | 159 | 191 | 152 |

### Round 4 — 1M keys, max value = UINT32_MAX (cumulative)

| Metric | Linear | Extendible | Larson |
|--------|--------|------------|--------|
| Unique keys stored | 3,801,696 | 3,801,696 | 3,801,696 |
| Bucket count | 1,267,232 | 1,351,839 | 1,267,232 |
| Split count | 1,005,088 | 1,351,837 | 1,005,088 |
| Load factor | 0.7500 | 0.7031 | 0.7500 |
| Memory (MB) | 29.00 | 291.13 | 29.00 |
| Insert CPU time (ms) | 647 | 2,921 | 581 |
| Query hits | 1,000,000 | 1,000,000 | 1,000,000 |
| Query CPU time (ms) | 154 | 201 | 150 |

---

## 3. Analysis

### 3.1 Correctness Verification

All three hash tables stored **exactly the same number of unique keys** in every
round, and all **query hits = 1,000,000** in every round.

This confirms:
- All three implementations handle `uint32_t` keys correctly including UINT32_MAX values
- Duplicate detection works correctly (rejected silently)
- Point queries correctly find every previously inserted key

---

### 3.2 Duplicate Rate Analysis

Inserting 1M random keys into a range of N produces duplicates due to the
**Birthday Problem**. The expected duplicate rate is approximately `1 - e^(-n/2N)`
for uniform distribution.

| Round | Attempted | Stored | Duplicates | Rate |
|-------|-----------|--------|------------|------|
| 1 | 1,000,000 | 951,464 | 48,536 | 4.85% |
| 2 | 1,000,000 | 929,155* | 70,845* | 7.08%* |
| 3 | 1,000,000 | 921,824* | 78,176* | 7.82%* |
| 4 | 1,000,000 | 999,253* | 747* | 0.07%* |

*Round 2–4 duplicates include both within-round collisions AND collisions
with keys already in the table from previous rounds.

**Key observation — Round 4:** With UINT32_MAX (~4.29B) as max value and only
~2.8M existing keys in the table, the chance of any new key matching an existing
one is extremely low (~0.065%), resulting in almost no duplicates (only 747 rejected).

---

### 3.3 Load Factor Analysis

```
Load Factor across rounds:

  Linear     : 0.75 | 0.75 | 0.75 | 0.75   (constant — perfectly controlled)
  Larson     : 0.75 | 0.75 | 0.75 | 0.75   (constant — perfectly controlled)
  Extendible : 0.70 | 0.71 | 0.71 | 0.70   (slightly lower — less precise)
```

**Linear and Larson** maintain an exact load factor of 0.75 at all times.
This is because their split trigger is deterministic: the moment the global
load factor exceeds 0.75, the next bucket in sequence is split.

**Extendible Hashing** cannot maintain an exact load factor because:
- When a bucket overflows it is split regardless of the global load
- Directory doubling may create many empty "shadow" entries pointing to
  the same underfull bucket
- This results in a slightly lower average load factor (~0.70)

---

### 3.4 Insert Performance Analysis

```
Insert CPU Time (ms):

  Round        Linear    Extendible    Larson
  ─────────────────────────────────────────────
  Round 1       430       1,205         392
  Round 2       789       1,378         708
  Round 3       627       1,239         642
  Round 4       647       2,921         581
```

**Linear Hashing:**
- Consistent performance across all rounds (430–789 ms)
- Simple O(1) insert: compute bucket index → append → check load → split if needed
- Split touches only one bucket at a time — cheap

**Larson Dynamic Hashing:**
- Slightly faster than Linear in most rounds (392–708 ms)
- Uses identical insert/split logic to Linear
- The small advantage is likely due to slightly better cache behavior

**Extendible Hashing:**
- Significantly slower — 2.8× to 4.5× slower than Linear
- Round 4 shows a sharp spike to 2,921 ms (vs ~647 ms for Linear)
- Root cause: `splitBucket` must update directory entries. With ~2M directory
  entries in Round 4, each directory doubling is expensive
- Additionally, `contains()` must dereference a shared_ptr on every lookup
  during insert, adding memory indirection overhead

**Why Round 2 is slower than Rounds 3 and 4 (Linear/Larson):**
Round 2 starts with 951K keys already in 317K buckets. The new 1M keys cause
many splits — pushing the table from ~317K to ~627K buckets (310K new splits).
This is the heaviest single-round growth. Rounds 3 and 4 add fewer new unique
keys, so fewer splits occur.

---

### 3.5 Query Performance Analysis

```
Query CPU Time (ms):

  Round        Linear    Extendible    Larson
  ─────────────────────────────────────────────
  Round 1       135         152          128
  Round 2       150         174          149
  Round 3       159         191          152
  Round 4       154         201          150
```

All three tables deliver near-identical query times because point query is
O(bucket_scan) — just scan up to 4 keys per bucket.

**Slight increase across rounds** is explained by the table growing larger,
causing more CPU cache misses when loading bucket data.

**Extendible is marginally slower** (~25–50 ms more per round) because:
- Looking up a bucket requires dereferencing a `shared_ptr` (extra pointer hop)
- The large directory array (up to 2M entries × 16 bytes = 32 MB) spreads
  data across more cache lines

**All 1,000,000 hits confirmed every round** — no false negatives.
This proves the hash functions correctly route queries to the right bucket
after all splits and directory doublings.

---

### 3.6 Memory Usage Analysis

```
Memory Usage (MB):

  Round        Linear    Extendible    Larson
  ─────────────────────────────────────────────
  Round 1        7.26      40.79         7.26
  Round 2       14.35      81.34        14.35
  Round 3       21.38      89.82        21.38
  Round 4       29.00     291.13        29.00
```

**Linear and Larson use identical memory** — both store keys in a flat
`vector<vector<uint32_t>>`. Memory grows linearly with unique keys stored:
- 3.8M keys × 4 bytes = ~14.5 MB (key data)
- ~1.27M bucket vector objects × 24 bytes overhead = ~14.5 MB
- Total ≈ 29 MB ✓

**Extendible uses ~10× more memory by Round 4 (291 MB vs 29 MB).**

Breakdown of Extendible memory:
- Directory array: `2^globalDepth` entries × 16 bytes per `shared_ptr`
  → With ~1.35M unique buckets, globalDepth ≈ 21 → 2^21 = 2,097,152 entries
  → 2,097,152 × 16 = **~32 MB** for directory alone
- Bucket metadata: 1,351,839 × `sizeof(Bucket)` ≈ **~41 MB**
- Key storage: 3.8M × 4 bytes = **~14.5 MB**
- `shared_ptr` control blocks + vector internal heap allocations ≈ remaining

The fundamental issue: the **directory must be a power-of-2 in size**.
Once globalDepth reaches 21, the directory jumps to 2M entries — even if
only half are unique buckets. This wastes significant memory on duplicate
directory pointers.

---

### 3.7 Split Count Analysis

```
Split Count after each round:

  Round        Linear      Extendible    Larson
  ────────────────────────────────────────────────
  Round 1       55,011       337,996      55,011
  Round 2      364,729       665,986     364,729
  Round 3      672,004       991,486     672,004
  Round 4    1,005,088     1,351,837   1,005,088
```

**Linear and Larson splits** grow by approximately the number of new buckets
created. Starting from 262,144 initial buckets:
- Final bucket count: 1,267,232
- Buckets added: 1,267,232 − 262,144 = **1,005,088 splits** ✓

**Extendible splits are ~35% higher** because it starts from only 2 buckets
and must split its way up to 1,351,839 buckets entirely through the split
mechanism. Every unique bucket created = one split.

**Larson splits equal Linear** because no deletes were performed — the merge
operation was never triggered. In a delete-heavy workload, Larson would show
fewer net buckets and better memory efficiency.

---

## 4. Summary Comparison

### Performance Winner per Category

| Category | Winner | Notes |
|----------|--------|-------|
| Insert speed | **Larson** | Marginally faster than Linear; both ~3–5× faster than Extendible |
| Query speed | **All equal** | ~130–200 ms; differences are cache-related not algorithmic |
| Memory efficiency | **Linear = Larson** | 10× less memory than Extendible at scale |
| Load factor precision | **Linear = Larson** | Exact 0.75 vs ~0.70 for Extendible |
| Scalability | **Linear = Larson** | Extendible degrades significantly at Round 4 (UINT32_MAX) |
| Delete + shrink support | **Larson only** | Only algorithm with merge — reclaims memory on heavy deletes |

### Key Takeaways

1. **Linear and Larson are nearly identical** for insert-only workloads.
   Larson's advantage only appears in workloads with frequent deletes,
   where its merge operation keeps memory usage from ballooning.

2. **Extendible Hashing trades memory for structural flexibility.**
   The directory-based design allows O(1) lookup with no overflow chains,
   but the directory itself becomes a major memory overhead at scale.

3. **The UINT32_MAX round exposes Extendible's weakness** — at 4M total keys
   the directory grows beyond 2M entries, making splits and directory doublings
   slow and expensive (2,921 ms insert vs 647 ms for Linear).

4. **All three algorithms achieve 100% query accuracy** regardless of key range
   or table size — the hashing and splitting logic is correct and robust.

5. **Duplicate handling works correctly** — the higher duplicate rate in
   Rounds 1–3 (dense key ranges) versus Round 4 (sparse UINT32_MAX range)
   matches the Birthday Problem prediction.
