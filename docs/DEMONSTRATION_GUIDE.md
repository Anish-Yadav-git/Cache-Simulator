# Cache Simulator - Functionality Verification Guide

This guide demonstrates how to verify that your cache simulator is running perfectly and showcases all available functionalities.

## 🚀 Quick Verification

### 1. Run Automated Test Suite
```bash
# Run the comprehensive test script
./test_comprehensive.sh
```

This script tests all 12 major functionality areas and provides colored output showing success/failure.

### 2. Basic Functionality Check
```bash
# Run built-in test suite
make test

# Run with default configuration
make run
```

## 🔍 Manual Feature Verification

### Cache Types

#### Direct-Mapped Cache (Associativity = 1)
```bash
./build/cache_simulator -s 512 -b 32 -a 1 -v
# Verify: Output shows "Direct Mapped" and single block per set
```

#### Set-Associative Cache (Associativity = N)
```bash
./build/cache_simulator -s 1024 -b 32 -a 4 -v
# Verify: Output shows "4-way" and 4 blocks per set
```

#### Fully Associative Cache (Associativity = 0)
```bash
./build/cache_simulator -s 512 -b 32 -a 0 -v
# Verify: Output shows "Fully Associative" and 1 set with all blocks
```

### Replacement Policies

#### LRU (Least Recently Used)
```bash
./build/cache_simulator -r LRU -v
# Verify: Output shows "LRU" policy in configuration
```

#### FIFO (First In First Out)
```bash
./build/cache_simulator -r FIFO -v
# Verify: Output shows "FIFO" policy in configuration
```

#### Random Replacement
```bash
./build/cache_simulator -r RANDOM -v
# Verify: Output shows "Random" policy in configuration
```

### Write Policies

#### Write-Through Policy
```bash
./build/cache_simulator -w WRITE_THROUGH -v
# Verify: Output shows "Write-Through" and dirty bits remain 0 (D:0)
```

#### Write-Back Policy
```bash
./build/cache_simulator -w WRITE_BACK -v
# Verify: Output shows "Write-Back" and dirty bits set on writes (D:1)
```

### Write Miss Policies

#### Write-Allocate
```bash
./build/cache_simulator -m WRITE_ALLOCATE -v
# Verify: Write misses allocate cache blocks
```

#### No-Write-Allocate
```bash
./build/cache_simulator -m NO_WRITE_ALLOCATE -v
# Verify: Write misses go directly to memory without allocation
```

## 🎯 Advanced Feature Testing

### Custom Memory Access Patterns
```bash
# Test specific addresses and operations
./build/cache_simulator -A 0x1000,0x2000,0x3000,0x1000 -O READ,WRITE,READ,WRITE -v

# Verify: 
# - Address 0x1000 first access: MISS
# - Address 0x1000 second access: HIT (shows caching works)
# - Statistics show correct hit/miss ratios
```

### Verbose Output Verification
```bash
./build/cache_simulator -v

# Verify output includes:
# - "Accessing address X with operation Y"
# - Cache Contents section showing [V:X D:X Tag:X] format
# - Detailed statistics breakdown
```

### Interactive Mode Testing
```bash
./build/cache_simulator --interactive

# Test these commands:
cache> access 0x100 read    # Should show MISS
cache> access 0x100 write   # Should show HIT  
cache> stats                # Should show statistics
cache> config               # Should show configuration
cache> contents             # Should show cache state
cache> reset                # Should clear cache
cache> batch 0x100,0x200 read,write  # Batch operations
cache> help                 # Show available commands
cache> quit                 # Exit
```

## 📊 Output Verification Checklist

### ✅ Configuration Display
- [x] Cache size, block size, associativity correctly shown
- [x] Number of sets and blocks calculated correctly
- [x] Bit breakdowns (offset, index, tag) accurate
- [x] Policies displayed correctly

### ✅ Memory Access Simulation
- [x] Addresses displayed in hex format
- [x] Operations (READ/WRITE) shown correctly
- [x] Results (HIT/MISS/WRITE HIT/WRITE MISS) accurate
- [x] Hit rates improve with repeated accesses

### ✅ Statistics Accuracy
- [x] Total accesses count correctly
- [x] Hit/miss counts accurate
- [x] Hit rate percentages correct
- [x] Read/write breakdowns separate
- [x] Statistics reset properly

### ✅ Cache Contents Display
- [x] Valid bits (V:0/1) shown correctly
- [x] Dirty bits (D:0/1) for write-back policy
- [x] Tags displayed in hex format
- [x] Invalid blocks marked clearly
- [x] Set organization visible

## 🧪 Error Handling Verification

### Invalid Configurations
```bash
# Test invalid cache size
./build/cache_simulator -s 0
# Should show: "Error: Cache size and block size must be greater than 0"

# Test mismatched sizes
./build/cache_simulator -s 100 -b 32
# Should show: "Error: Cache size must be a multiple of block size"
```

### Invalid Arguments
```bash
# Test unknown option
./build/cache_simulator --invalid-option
# Should show: "Unknown option. Use --help for usage information."
```

## 🎪 Performance Testing

### Large Configuration Test
```bash
# Test with large cache (64KB, 16-way)
./build/cache_simulator -s 65536 -b 64 -a 16 -v

# Verify:
# - Configuration calculates correctly
# - Performance remains responsive
# - Memory access patterns work
# - Statistics accurate for large numbers
```

### Stress Testing
```bash
# Test with many addresses
./build/cache_simulator -A 0x0,0x1000,0x2000,0x3000,0x4000,0x5000,0x6000,0x7000 -O READ,READ,READ,READ,READ,READ,READ,READ -v
```

## 🎭 Interactive Mode Feature Matrix

| Command | Functionality | Verification |
|---------|---------------|--------------|
| `access <addr> <op>` | Single memory access | Shows correct HIT/MISS |
| `batch <addrs> <ops>` | Multiple accesses | Processes all correctly |
| `stats` | Show statistics | Displays current stats |
| `reset` | Clear cache | Zeros all statistics |
| `config` | Show configuration | Displays current config |
| `contents` | Show cache state | Shows all cache blocks |
| `help` | Show commands | Lists all commands |
| `quit` | Exit mode | Cleanly exits |

## 📈 Expected Results Summary

Your cache simulator is working perfectly if:

1. **All 12 test categories pass** in the comprehensive test
2. **Cache types behave differently** with same input patterns
3. **Replacement policies** show expected victim selection
4. **Write policies** correctly manage dirty bits
5. **Statistics** accurately reflect cache behavior
6. **Interactive mode** responds to all commands
7. **Error handling** catches invalid inputs
8. **Performance** scales with larger configurations
9. **Help system** provides comprehensive information
10. **Verbose output** shows detailed operation traces

## 🎉 Success Indicators

✅ **Test suite shows all green checkmarks**  
✅ **Hit rates improve with locality of reference**  
✅ **Different cache configurations produce different results**  
✅ **Dirty bits appear only with write-back policy**  
✅ **Interactive mode responds correctly to all commands**  
✅ **Error messages appear for invalid inputs**  
✅ **Cache contents display shows valid cache organization**  
✅ **Statistics reset and accumulate correctly**  
✅ **Help system provides complete usage information**  
✅ **Large configurations work without issues**

If all these indicators are met, your cache simulator is functioning perfectly with all features working as designed!

## 🔧 Troubleshooting

If any test fails:
1. Check build environment: `make clean && make`
2. Verify file permissions: `chmod +x test_comprehensive.sh`
3. Check dependencies: Ensure C++17 compiler and CMake 3.14+
4. Review error messages for specific issues
5. Test individual components before running full suite
