
# Week 5 - Priority Flow Correctness

## Objective

Maximize delivered power lexicographically:

(F1, F2, F3)

F1: Priority 1 delivered MW
F2: Priority 2 delivered MW
F3: Priority 3 delivered MW

## Test A - Original Redundant Grid

Generator supply: 60 MW

Expected priority totals:
P1 = 20 MW
P2 = 10 MW
P3 = 30 MW

Expected total: 60 MW

## Test B - Critical Consumer vs Throughput

Construct a network where:

- Allocation A delivers 10 MW to P1.
- Allocation B delivers 20 MW to P3.
- Both allocations are feasible.
- Network constraints prevent combining them.

Expected lexicographic preference:

Allocation A is better than B.

## Test C - Same-Priority Redistribution

Construct a network with two P1 consumers.

An earlier P1 allocation must be redistributed
to improve P2 delivery while keeping the
optimal P1 total unchanged.

## Test D - Failed Transmission Line

Verify that inactive edges carry zero flow.

## Test E - Shared Undirected Capacity

For every physical line:

-C <= signedFlow <= C

## Test F - Flow Conservation

At every intermediate node:

Total inflow = Total outflow

## Completion Criteria

Do not mark the optimizer complete until:

1. Priority totals are lexicographically optimal.
2. Supply and demand constraints hold.
3. Transmission capacities are respected.
4. Residual rerouting is correct.
5. Invalid inputs are rejected safely.
6. Earlier Week 1-4 tests still pass.
