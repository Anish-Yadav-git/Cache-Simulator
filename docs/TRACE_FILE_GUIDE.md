# Cache Simulator - Trace File and Output Guide

This guide explains how to use the cache simulator with trace files and output statistics to files.

## New Features

### 1. Trace File Input (`-t, --trace-file`)
- Read memory access patterns from a file instead of command line
- Supports standard trace file formats used in computer architecture research
- Ideal for batch processing and reproducible experiments

### 2. Statistics Output (`-o, --output-file`)
- Write detailed statistics to a file (default: `stats.txt`)
- Comprehensive report including configuration, access details, and performance metrics
- Perfect for analysis and comparison of different cache configurations

### 3. Quiet Mode (`-q, --quiet`)
- Suppress console output for batch processing
- Only essential messages are shown
- Ideal when processing large trace files or running automated experiments

## Trace File Format

### Basic Format
Each line represents one memory access:
```
<operation> <address>
```

### Supported Operations
- `R` or `READ` - Read operation
- `W` or `WRITE` - Write operation

### Address Formats
- Hexadecimal with 0x prefix: `0x1000`
- Hexadecimal without prefix: `1000`
- Decimal: `4096`

### Example Trace File
```
# Sample trace file
# Comments start with #

# Sequential array initialization
W 0x10000000
W 0x10000004
W 0x10000008
W 0x1000000C

# Read operations
R 0x10000000
R 0x10000004
READ 0x20000000
WRITE 0x20000004
```

## Usage Examples

### Basic Trace File Processing
```bash
# Process trace file with default cache configuration
./build/cache_simulator -t memory_trace.txt

# Process with custom cache configuration
./build/cache_simulator -t trace.txt -s 2048 -b 64 -a 8 -r LRU
```

### Batch Processing (Quiet Mode)
```bash
# Run quietly and save to custom output file
./build/cache_simulator -t large_trace.txt -o results.txt -q

# Compare different configurations quietly
./build/cache_simulator -t trace.txt -o lru_results.txt -r LRU -q
./build/cache_simulator -t trace.txt -o fifo_results.txt -r FIFO -q
./build/cache_simulator -t trace.txt -o random_results.txt -r RANDOM -q
```

### Detailed Analysis (Verbose Mode)
```bash
# Get detailed output with cache contents
./build/cache_simulator -t trace.txt -o detailed_stats.txt -v

# Analyze with write-back policy
./build/cache_simulator -t trace.txt -w WRITE_BACK -v
```

## Output File Format

The output file contains the following sections:

### 1. Header
- Generation timestamp
- Cache configuration details

### 2. Simulation Details
- Total memory accesses
- Simulation time
- Input trace file name

### 3. Cache Statistics
- Hit/miss rates overall and by operation type
- Read/write performance breakdown

### 4. Access Details (for small traces or verbose mode)
- Step-by-step access results
- Address and operation for each access

### 5. Performance Summary
- Accesses per second
- Average access time

### Sample Output File
```
========================================
Cache Simulator Statistics Report
========================================
Generated: 2025-07-06 18:11:16

CACHE CONFIGURATION:
-------------------
Cache Configuration:
  Cache Size: 1024 bytes
  Block Size: 32 bytes
  Associativity: 4-way
  Number of Sets: 8
  Number of Blocks: 32
  Offset Bits: 5
  Index Bits: 3
  Tag Bits: 56
  Replacement Policy: LRU
  Write Policy: Write-Through
  Write Miss Policy: Write-Allocate

SIMULATION DETAILS:
------------------
Total Memory Accesses: 23
Simulation Time: 0.000036 seconds
Input Trace File: sample_trace.txt

CACHE STATISTICS:
-----------------
Cache Statistics:
  Total Accesses: 23
  Hits: 6
  Misses: 17
  Hit Rate: 26.09%
  Miss Rate: 73.91%

  Read Accesses: 17
  Write Accesses: 6
  Read Hit Rate: 17.65%
  Write Hit Rate: 50.00%

  Read Hits: 3
  Read Misses: 14
  Write Hits: 3
  Write Misses: 3

ACCESS DETAILS:
--------------
     1: 0x00001000 ( READ) -> MISS
     2: 0x00001004 ( READ) -> HIT
     3: 0x00001008 ( read) -> HIT
     ...

PERFORMANCE SUMMARY:
-------------------
Accesses per second: 638889
Average access time: 1.565 microseconds

========================================
End of Report
========================================
```

## Use Cases

### 1. Academic Research
- Analyze real program traces
- Compare cache configurations systematically
- Generate data for research papers

### 2. Performance Analysis
- Evaluate cache behavior for specific applications
- Identify performance bottlenecks
- Optimize cache parameters

### 3. Educational Purposes
- Demonstrate cache concepts with real traces
- Create assignments with different trace files
- Show impact of various cache configurations

### 4. Batch Experiments
- Run multiple configurations automatically
- Process large trace files efficiently
- Generate comparative reports

## Tips for Creating Trace Files

### 1. Real Program Traces
- Use debugging tools or simulators to capture real memory accesses
- Include both instruction and data accesses if relevant
- Consider different program phases

### 2. Synthetic Traces
- Create patterns to test specific cache behaviors
- Sequential accesses for spatial locality testing
- Repeated accesses for temporal locality testing
- Random accesses for worst-case analysis

### 3. Large Traces
- Use quiet mode (`-q`) for large files
- Consider sampling for very large traces
- Monitor memory usage during processing

## Command Reference

### New Options
```
-t, --trace-file FILE      Input trace file with memory accesses
-o, --output-file FILE     Output statistics file (default: stats.txt)
-q, --quiet                Suppress console output
```

### Complete Example
```bash
./build/cache_simulator \
  --trace-file program_trace.txt \
  --output-file analysis_results.txt \
  --cache-size 4096 \
  --block-size 64 \
  --associativity 8 \
  --replacement LRU \
  --write-policy WRITE_BACK \
  --quiet
```

## Integration with Existing Features

The trace file functionality works seamlessly with all existing features:
- All cache configurations (direct-mapped, set-associative, fully associative)
- All replacement policies (LRU, FIFO, Random)
- All write policies (Write-through, Write-back)
- Verbose output and interactive mode

This makes the cache simulator a powerful tool for both educational purposes and research applications.
