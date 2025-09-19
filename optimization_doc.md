# Ant Mania – Optimization Report  

## Thought Process  
When I started this challenge, my main priority was minimizing simulation latency once the map was loaded.  
I focused on a design that is simple, cache-friendly, and predictable rather than over-engineered.  
The hot path (ant movement and collision detection) was the main target for optimization, since that dominates runtime.  

---

## Optimization Steps  

1. **Integer IDs instead of strings**  
   - Colony names parsed once at load time.  
   - All later lookups are integer-based, avoiding costly string comparisons.  

2. **Fixed-size arrays for connections**  
   - Each colony has at most 4 neighbors (N/S/E/W).  
   - Used `std::array<uint32_t, 4>` for compact, cache-friendly storage.  

3. **Three-pass map parsing**  
   - Pass 1: Count colonies and reserve memory.  
   - Pass 2: Assign integer IDs to names.  
   - Pass 3: Resolve connections using IDs.  
   - Guarantees valid lookups and avoids string handling during simulation.  

4. **Zero allocations in the hot path**  
   - Preallocated vectors for collision tracking.  
   - Stack-allocated arrays for possible moves.  
   - No dynamic memory allocation inside the simulation loop.  

5. **Incremental termination check**  
   - Track alive ants and max-move ants.  
   - Early termination becomes O(1) instead of scanning all ants.  

6. **Efficient random number generation**  
   - Prebuilt `std::uniform_int_distribution` objects for 1–4 choices.  
   - Avoids modulo bias and repeated setup cost.  

7. **Batch processing**  
   - Ants are moved in groups of 8.  
   - Improves cache locality and reduces branch mispredictions.  

---

## Benchmark Results  

### Small Map   

| Ants | Time (ms) |
|------|-----------|
| 50   | 12.72     |
| 100  | 8.40      |
| 200  | 6.85      |  

**Average:** 9.32 ms  

With more ants, collisions happen earlier → fewer iterations.  

---

### Medium Map 

| Ants | Time (ms) |
|------|-----------|
| 100  | 89.15     |
| 500  | 75.20     |
| 1000 | 79.83     |
| 2000 | 87.26     |  

**Average:** 82.86 ms  

Runtime stays stable around ~80 ms regardless of ant count.  
Scaling is good: performance is driven by iterations, not just ants.  

---

## Complexity  

- **Time complexity:**  
  - Movement step → O(A) per iteration.  
  - Collision detection → O(A + C) per iteration.  
  - **Overall:** O(I × (A + C)), where A = ants, C = colonies, I = iterations (≤ MAX_MOVES).  

- **Space complexity:**  
  - Ant storage → O(A).  
  - Colony graph → O(C).  
  - Collision buffers → O(C).  
  - **Overall:** O(A + C).  

---

## Multithreading Consideration  

I kept the solution **single-threaded** because:  
- For up to a few thousand ants, thread overhead is greater than the benefit.  
- Collision detection is global, so threading would require synchronization or merging.  
- Single-threaded execution ensures predictable latency and clean benchmarks.  

If scaled to **hundreds of thousands or millions of ants**, multithreading could help:  
- Ant movement is embarrassingly parallel → easy to split across threads.  
- Collision detection could use per-thread local buffers + merge or atomics.  

---

## Conclusion  

The solution is:  
- **Cache-friendly** – compact memory layout, batched processing.  
- **Allocation-free in hot paths** – no heap allocations during simulation.  
- **Efficient** –  
  - Small maps: single-digit milliseconds.  
  - Medium maps: ~80 ms even with 2000 ants.  
