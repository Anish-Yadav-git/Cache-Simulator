#!/bin/bash

# Cache Write Operations Demo Script
# This script demonstrates the difference between write-through and write-back policies

echo "=== Cache Write Operations Demonstration ==="
echo

# Build the project first
echo "Building cache simulator..."
make clean && make
echo

# Cache configuration
CACHE_SIZE=1024
BLOCK_SIZE=32
ASSOCIATIVITY=4
TRACE_FILE="examples/traces/write_demo.txt"

echo "Cache Configuration:"
echo "- Size: ${CACHE_SIZE} bytes"
echo "- Block Size: ${BLOCK_SIZE} bytes"
echo "- Associativity: ${ASSOCIATIVITY}-way"
echo "- Trace File: ${TRACE_FILE}"
echo

# Demo 1: Write-Through Policy
echo "=========================================="
echo "DEMO 1: WRITE-THROUGH POLICY"
echo "- Writes update both cache and memory immediately"
echo "- Dirty bits should always be 0 (D:0)"
echo "=========================================="
echo

./bin/cache_simulator -s ${CACHE_SIZE} -b ${BLOCK_SIZE} -a ${ASSOCIATIVITY} \
    -w WRITE_THROUGH -m WRITE_ALLOCATE -t ${TRACE_FILE} -v

echo
echo "Press Enter to continue to Write-Back demo..."
read

# Demo 2: Write-Back Policy
echo "=========================================="
echo "DEMO 2: WRITE-BACK POLICY"
echo "- Writes update only cache, memory updated on eviction"
echo "- Dirty bits set to 1 (D:1) for modified blocks"
echo "=========================================="
echo

./bin/cache_simulator -s ${CACHE_SIZE} -b ${BLOCK_SIZE} -a ${ASSOCIATIVITY} \
    -w WRITE_BACK -m WRITE_ALLOCATE -t ${TRACE_FILE} -v

echo
echo "Press Enter to continue to Write-Back with No-Write-Allocate demo..."
read

# Demo 3: Write-Back with No-Write-Allocate
echo "=========================================="
echo "DEMO 3: WRITE-BACK + NO-WRITE-ALLOCATE"
echo "- Write misses don't load block into cache"
echo "- Only write hits create dirty blocks"
echo "=========================================="
echo

./bin/cache_simulator -s ${CACHE_SIZE} -b ${BLOCK_SIZE} -a ${ASSOCIATIVITY} \
    -w WRITE_BACK -m NO_WRITE_ALLOCATE -t ${TRACE_FILE} -v

echo
echo "=========================================="
echo "SUMMARY:"
echo "- Write-Through: All blocks have D:0 (never dirty)"
echo "- Write-Back: Written blocks have D:1 (dirty until eviction)"
echo "- No-Write-Allocate: Fewer cache blocks, only hits create dirty blocks"
echo "=========================================="
