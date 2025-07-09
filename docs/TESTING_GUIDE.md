# Cache Simulator - Testing Guide

This guide explains how to use the comprehensive test suite and interpret the results.

## 🧪 Running the Test Suite

### Quick Start
```bash
# Run the comprehensive test suite
./tests/test_comprehensive.sh

# Or via make
make test
```

### What the Test Suite Does

The comprehensive test suite validates **13 categories** of functionality:

1. **Basic Functionality Test** - Runs the unit test suite
2. **Default Configuration Test** - Validates default cache behavior
3. **Cache Type Tests** - Direct-mapped, Set-associative, Fully associative
4. **Replacement Policy Tests** - LRU, FIFO, Random policies
5. **Write Policy Tests** - Write-through vs Write-back with dirty bit verification
6. **Write Miss Policy Tests** - Write-allocate vs No-write-allocate
7. **Custom Memory Access Pattern Tests** - User-defined address sequences
8. **Trace File Tests** - External trace file processing
9. **Verbose Output Test** - Detailed cache contents display
10. **Interactive Mode Test** - Command-line interactive interface
11. **Help System Test** - Documentation and usage information
12. **Error Handling Tests** - Invalid configuration detection
13. **Performance Test** - Large cache configurations

## 📊 Test Results and Output

### Console Output
The test suite provides real-time feedback with:
- **🔵 Blue headers** for test sections
- **🟡 Yellow arrows** for individual tests
- **🟢 Green checkmarks** for successful tests
- **🔴 Red X marks** for failed tests

### Generated Files

#### Main Results File
- **Location**: `examples/output/comprehensive_test_stats.txt`
- **Content**: Summary of all test results with timestamps
- **Format**: Human-readable test report

#### Individual Test Outputs
- **Location**: `examples/output/test_outputs/`
- **Files**: Separate `.txt` and `.out` files for each test
- **Content**: Detailed statistics and console output for each test

### Sample Test Report

```
========================================
Cache Simulator Comprehensive Test Report
========================================
Generated: Mon Jul  7 12:55:49 IST 2025

1. BASIC FUNCTIONALITY TEST
===========================
✓ All basic tests passed!

2. DEFAULT CONFIGURATION TEST
=============================
✓ Default configuration working correctly!

3. CACHE TYPE TESTS
===================
✓ Direct-mapped cache working!
✓ Fully associative cache working!
✓ 8-way set associative cache working!

... (continues for all 13 test categories)

TEST SUMMARY
============

Test completed at: Mon Jul  7 12:55:55 IST 2025

All test outputs saved to: examples/output/test_outputs
Full test report saved to: examples/output/comprehensive_test_stats.txt
```

## 🔍 Understanding Test Results

### Successful Test Run
- **All checkmarks (✓)** - Every test passed
- **Complete stats file** - Generated at specified location
- **Individual output files** - One for each test category
- **Exit code 0** - Script completed successfully

### Failed Test Indicators
- **Red X marks (✗)** - Specific test failed
- **Debug information** - Included in stats file for failed tests
- **Exit code 1** - Script stopped on first critical failure

### Key Validation Points

#### Cache Types
- **Direct-mapped**: Associativity = 1, single block per set
- **Set-associative**: Associativity = N, N blocks per set
- **Fully associative**: Associativity = 0, all blocks in one set

#### Replacement Policies
- **LRU**: Least Recently Used eviction
- **FIFO**: First In First Out eviction
- **Random**: Random block selection for eviction

#### Write Policies
- **Write-through**: Immediate write to memory, dirty bits stay 0
- **Write-back**: Delayed write to memory, dirty bits (D:1) appear

#### Error Handling
- **Invalid cache size**: Size must be > 0
- **Mismatched sizes**: Cache size must be multiple of block size

## 🎯 Using Test Results

### For Development
- **Verify functionality** after code changes
- **Catch regressions** in cache behavior
- **Validate new features** work correctly

### For Education
- **Demonstrate concepts** with working examples
- **Show different behaviors** of cache configurations
- **Provide concrete evidence** of cache performance

### For Research
- **Baseline verification** before experiments
- **Configuration validation** for specific studies
- **Performance benchmarking** across different setups

## 🔧 Customizing Tests

### Adding New Tests
1. **Create test function** in the script
2. **Add to main test sequence**
3. **Include result logging** to stats file
4. **Update section numbering**

### Modifying Test Parameters
```bash
# Edit test_comprehensive.sh to change:
# - Cache sizes and configurations
# - Memory access patterns
# - Output file locations
# - Test validation criteria
```

### Test Configuration Examples
```bash
# Test with different cache sizes
./bin/cache_simulator -s 512 -o test_512.txt
./bin/cache_simulator -s 2048 -o test_2048.txt
./bin/cache_simulator -s 8192 -o test_8192.txt

# Test with different associativities
./bin/cache_simulator -a 1 -o direct_mapped.txt
./bin/cache_simulator -a 4 -o four_way.txt
./bin/cache_simulator -a 8 -o eight_way.txt

# Test with different policies
./bin/cache_simulator -r LRU -o lru_test.txt
./bin/cache_simulator -r FIFO -o fifo_test.txt
./bin/cache_simulator -r RANDOM -o random_test.txt
```

## 📈 Performance Analysis

### Using Test Results for Optimization
1. **Compare hit rates** across configurations
2. **Analyze access patterns** for your workload
3. **Identify optimal settings** for specific use cases
4. **Validate theoretical predictions** with simulation

### Example Analysis Workflow
```bash
# Run comprehensive tests
./tests/test_comprehensive.sh

# Analyze specific configurations
./bin/cache_simulator -t examples/traces/program_trace.txt -s 1024 -a 4 -r LRU -o analysis_1.txt
./bin/cache_simulator -t examples/traces/program_trace.txt -s 2048 -a 8 -r LRU -o analysis_2.txt

# Compare results
grep "Hit Rate:" analysis_*.txt
```

## 🎓 Educational Use

### For Students
- **Run tests** to see cache behavior
- **Modify parameters** to observe changes
- **Study output files** to understand cache mechanics
- **Create custom traces** for specific experiments

### For Instructors
- **Demonstrate concepts** with working examples
- **Assign exercises** using different configurations
- **Grade implementations** using the test suite
- **Show real performance** impact of design decisions

## 🚨 Troubleshooting

### Common Issues
1. **Permission errors**: Ensure test script is executable (`chmod +x`)
2. **Missing files**: Run `make` to build binaries first
3. **Output directory**: Ensure `examples/output/` exists
4. **Path issues**: Run from project root directory

### Debug Information
- **Failed tests** include debug output in stats file
- **Individual files** show detailed error messages
- **Console output** provides real-time feedback
- **Exit codes** indicate success (0) or failure (1)

The comprehensive test suite ensures your cache simulator is working correctly across all supported features and configurations!
