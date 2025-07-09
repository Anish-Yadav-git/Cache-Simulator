# Cache Simulator

A comprehensive, modular C++ cache simulator that models real computer memory cache behavior with support for various cache configurations, replacement policies, and write strategies.

## ✨ Features

- **🏗️ Multiple Cache Types**: Direct-mapped, Set-associative, Fully associative
- **🔄 Replacement Policies**: LRU (Least Recently Used), FIFO (First In First Out), Random
- **✍️ Write Policies**: Write-through, Write-back with dirty bit tracking
- **📝 Write Miss Policies**: Write-allocate, No-write-allocate
- **📊 Detailed Statistics**: Hit rate, miss rate, read/write breakdown with performance analysis
- **🧩 Modular Design**: Easy to extend with new policies and configurations
- **🎯 Educational Focus**: Perfect for learning computer architecture concepts
- **🔧 Debug Support**: Cache contents visualization and comprehensive logging
- **⌨️ Command Line Interface**: Comprehensive CLI with configuration options
- **🎮 Interactive Mode**: Real-time cache experimentation and testing
- **📋 Batch Processing**: Support for custom memory access patterns

## 📁 Project Structure

```
Cache Simulator/
├── 📁 include/              # Header files (.h)
│   ├── cache.h             # Base cache interface
│   ├── cache_statistics.h  # Statistics tracking
│   ├── replacement_policy.h # Replacement policy interface
│   └── set_associative_cache.h # Main cache implementation
├── 📁 src/                  # Implementation files (.cpp)
│   ├── cache.cpp           # Base cache implementation
│   ├── cache_statistics.cpp # Statistics implementation
│   ├── replacement_policy.cpp # Policy implementations
│   ├── set_associative_cache.cpp # Main cache logic
│   └── main.cpp            # Demo program
├── 📁 tests/                # Test files
│   └── cache_test.cpp      # Comprehensive test suite
├── 📁 build/                # Build directory (auto-generated)
├── 📁 docs/                 # Documentation
├── 📄 CMakeLists.txt        # CMake build configuration
├── 📄 Makefile              # Convenience build wrapper
└── 📄 README.md             # This documentation
```

## 🚀 Building the Project

### Prerequisites
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.14** or higher
- **Make** utility

### Quick Start (Recommended)

The project includes a convenient Makefile wrapper for easy building and running:

```bash
# Build and run the simulator
make run

# Build and run tests
make test

# Show all available options
make help
```

### Available Make Targets

| Target | Description |
|--------|-------------|
| `make` or `make all` | Build the project (default) |
| `make build` | Build the cache simulator |
| `make run` | Build and run the main simulator |
| `make test` | Build and run the comprehensive test suite |
| `make debug` | Build debug version with symbols |
| `make clean` | Remove all build files |
| `make setup` | Setup build directory only |
| `make install` | Install binaries (optional) |
| `make help` | Show detailed help message |

### Manual CMake Build (Alternative)

If you prefer to use CMake directly:

```bash
# 1. Create and enter build directory
mkdir build && cd build

# 2. Configure the project
cmake ..

# 3. Build the project
make

# 4. Run executables
./cache_simulator  # Main demo
./cache_test       # Test suite
```

### Build Configurations

- **Release** (default): Optimized for performance (`-O2`)
- **Debug**: Includes debug symbols and assertions (`-g -O0 -DDEBUG`)

## 🚀 Quick Reference

```bash
# Default configuration
./build/cache_simulator

# Custom cache configuration
./build/cache_simulator -s 2048 -b 64 -a 8 -r LRU -w WRITE_BACK

# Custom memory access pattern
./build/cache_simulator -A 0x0,0x100,0x200 -O READ,WRITE,READ -v

# Interactive mode
./build/cache_simulator --interactive

# Show help
./build/cache_simulator --help
```

## 🎮 Usage

### Command Line Interface

The cache simulator provides a comprehensive command-line interface for configuring and testing different cache configurations.

#### Basic Usage
```bash
# Run with default configuration
./build/cache_simulator

# Or using make
make run
```

### Command Line Options

| Option | Short | Description | Default |
|--------|-------|-------------|----------|
| `--cache-size` | `-s` | Cache size in bytes | 1024 |
| `--block-size` | `-b` | Block size in bytes | 32 |
| `--associativity` | `-a` | Associativity (1=direct, 0=fully) | 4 |
| `--replacement` | `-r` | Replacement policy: LRU\|FIFO\|RANDOM | LRU |
| `--write-policy` | `-w` | Write policy: WRITE_THROUGH\|WRITE_BACK | WRITE_THROUGH |
| `--write-miss` | `-m` | Write miss: WRITE_ALLOCATE\|NO_WRITE_ALLOCATE | WRITE_ALLOCATE |
| `--trace-file` | `-t` | Input trace file with memory accesses | - |
| `--output-file` | `-o` | Output statistics file | stats.txt |
| `--addresses` | `-A` | Comma-separated hex addresses | Default pattern |
| `--operations` | `-O` | Comma-separated operations (READ\|WRITE) | Default pattern |
| `--interactive` | `-i` | Interactive mode | false |
| `--verbose` | `-v` | Verbose output | false |
| `--quiet` | `-q` | Suppress console output | false |
| `--help` | `-h` | Show help message | - |

#### Example Commands

```bash
# 8-way set associative cache with 2KB size
./build/cache_simulator --cache-size 2048 --associativity 8 --replacement LRU

# Direct-mapped cache with custom addresses
./build/cache_simulator -s 512 -b 16 -a 1 -r FIFO --addresses 0x0,0x10,0x20

# Fully associative cache with write-back policy
./build/cache_simulator -s 1024 -b 32 -a 0 -w WRITE_BACK -r LRU

# Process trace file with default settings
./build/cache_simulator -t memory_trace.txt

# Batch processing with custom output
./build/cache_simulator -t trace.txt -o results.txt -q

# Trace file with custom cache configuration
./build/cache_simulator -t program_trace.txt -s 4096 -b 64 -a 8 -r LRU -w WRITE_BACK

# Custom memory access pattern
./build/cache_simulator -A 0x100,0x200,0x300 -O READ,WRITE,READ -v

# Interactive mode for experimentation
./build/cache_simulator --interactive
```

### Interactive Mode

The interactive mode provides a command-line interface for real-time cache experimentation:

```bash
./build/cache_simulator --interactive
```

#### Interactive Commands

| Command | Description | Example |
|---------|-------------|----------|
| `access <addr> <op>` | Access memory address | `access 0x100 READ` |
| `batch <addrs> <ops>` | Batch memory accesses | `batch 0x0,0x20 read,write` |
| `stats` | Show current statistics | `stats` |
| `reset` | Reset cache and statistics | `reset` |
| `config` | Show cache configuration | `config` |
| `contents` | Show cache contents | `contents` |
| `help` | Show command help | `help` |
| `quit` | Exit interactive mode | `quit` |

### Help and Documentation

```bash
# Show detailed help message
./build/cache_simulator --help

# Quick help in interactive mode
cache> help
```

### Quick Testing

```bash
# Run comprehensive test suite
make test

# Clean build
make clean && make

# Build debug version
make debug
```

### Programmatic Usage

For integration into other projects:

```cpp
#include "set_associative_cache.h"
#include "replacement_policy.h"

// Create a 4-way set associative cache with LRU replacement
auto policy = ReplacementPolicyFactory::createPolicy(
    ReplacementPolicyFactory::PolicyType::LRU, 
    num_sets, 
    associativity
);

SetAssociativeCache cache(
    1024,  // 1KB cache size
    32,    // 32B block size
    4,     // 4-way associative
    std::move(policy)
);

// Access cache
auto result = cache.access(0x1000, Cache::Operation::READ);

// Get statistics
auto stats = cache.getStatistics();
std::cout << "Hit Rate: " << stats.getHitRate() << "%" << std::endl;
```

## Configuration Options

### Cache Types
- **Direct-mapped**: `associativity = 1`
- **Set-associative**: `associativity = N` (where N > 1)
- **Fully associative**: `associativity = 0` (automatically calculated)

### Replacement Policies
- `LRU` - Least Recently Used
- `FIFO` - First In First Out
- `RANDOM` - Random replacement

### Write Policies
- `WRITE_THROUGH` - Write to cache and memory simultaneously
- `WRITE_BACK` - Write to cache only, write to memory on eviction

### Write Miss Policies
- `WRITE_ALLOCATE` - Allocate cache block on write miss
- `NO_WRITE_ALLOCATE` - Write directly to memory on write miss

## 📊 Example Outputs

### Default Configuration (`make run`)
```
Cache Simulator CLI
==================
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

Using default test pattern.

Memory Access Simulation:
========================
Access 0x0 (READ) -> MISS
Access 0x20 (WRITE) -> WRITE MISS
Access 0x40 (read) -> MISS
Access 0x60 (WRITE) -> WRITE MISS
Access 0x80 (read) -> MISS
Access 0x100 (WRITE) -> WRITE MISS
Access 0x0 (read) -> HIT
Access 0x0 (WRITE) -> WRITE HIT

Cache Statistics:
  Total Accesses: 8
  Hits: 2
  Misses: 6
  Hit Rate: 25.00%
  Miss Rate: 75.00%

  Read Accesses: 4
  Write Accesses: 4
  Read Hit Rate: 25.00%
  Write Hit Rate: 25.00%

  Read Hits: 1
  Read Misses: 3
  Write Hits: 1
  Write Misses: 3
```

### Custom Configuration Example
```bash
./build/cache_simulator -s 2048 -a 8 -r FIFO -A 0x0,0x100,0x200,0x0 -O READ,read,write,read -v
```

```
Cache Simulator CLI
==================
Cache Configuration:
  Cache Size: 2048 bytes
  Block Size: 32 bytes
  Associativity: 8-way
  Number of Sets: 8
  Number of Blocks: 64
  Offset Bits: 5
  Index Bits: 3
  Tag Bits: 56
  Replacement Policy: FIFO
  Write Policy: Write-Through
  Write Miss Policy: Write-Allocate

Memory Access Simulation:
========================
Accessing address 0x0 with operation READ -> MISS
Accessing address 0x100 with operation READ -> MISS
Accessing address 0x200 with operation WRITE -> WRITE MISS
Accessing address 0x0 with operation READ -> HIT

Cache Statistics:
  Total Accesses: 4
  Hits: 1
  Misses: 3
  Hit Rate: 25.00%
  Miss Rate: 75.00%
  ...
```

### Interactive Mode Example
```bash
./build/cache_simulator --interactive
```

```
=== Interactive Mode ===
Commands:
  access <address> <READ|WRITE> - Access memory address
  batch <addr1,addr2,...> <op1,op2,...> - Batch access
  stats - Show current statistics
  reset - Reset cache and statistics
  config - Show cache configuration
  contents - Show cache contents
  help - Show this help
  quit - Exit interactive mode

cache> access 0x100 read
Access 0x100 (READ) -> MISS

cache> access 0x200 write
Access 0x200 (WRITE) -> WRITE MISS

cache> access 0x100 read
Access 0x100 (READ) -> HIT

cache> stats
Cache Statistics:
  Total Accesses: 3
  Hits: 1
  Misses: 2
  Hit Rate: 33.33%
  Miss Rate: 66.67%
  ...

cache> quit
```

### Test Suite Sample (`make test`)
```
Cache Simulator Test Suite
==========================

=== Testing Direct-Mapped Cache ===
Cache Configuration:
  Cache Size: 512 bytes
  Block Size: 32 bytes
  Associativity: Direct Mapped
  Number of Sets: 16
  Number of Blocks: 16
  ...

Access 0x0 -> MISS
Access 0x200 -> MISS
Access 0x0 -> MISS
Access 0x200 -> MISS

=== Testing Fully Associative Cache ===
...
Access 0x0 -> MISS
Access 0x200 -> MISS
Access 0x400 -> MISS
Access 0x600 -> MISS
Access 0x0 -> HIT
Access 0x200 -> HIT

=== Testing Different Replacement Policies ===
--- LRU Policy ---
Hit Rate: 33.33%

--- FIFO Policy ---
Hit Rate: 33.33%

--- RANDOM Policy ---
Hit Rate: 33.33%

All tests completed!
```

## Design Patterns Used

- **Strategy Pattern**: For replacement policies
- **Factory Pattern**: For creating replacement policies
- **Template Method**: For cache access flow
- **Observer Pattern**: For statistics collection

## Extending the Simulator

### Adding New Replacement Policies

1. Create a new class inheriting from `ReplacementPolicy`
2. Implement required virtual methods
3. Add the policy to `ReplacementPolicyFactory`

```cpp
class MyPolicy : public ReplacementPolicy {
public:
    size_t selectVictim(size_t set_index, const std::vector<bool>& valid_blocks) override;
    void updateOnAccess(size_t set_index, size_t block_index, bool hit) override;
    void reset() override;
    std::string getName() const override { return "MyPolicy"; }
};
```

### Adding New Cache Types

Inherit from the `Cache` base class and implement the required virtual methods.

## Performance Considerations

- The simulator is optimized for functionality over performance
- For large-scale simulations, consider:
  - Disabling debug output
  - Using release builds (`-O2` or `-O3`)
  - Minimizing statistics collection if not needed

## Testing

The project includes comprehensive tests covering:
- Direct-mapped cache behavior
- Set-associative cache behavior
- Fully associative cache behavior
- Different replacement policies
- Write policies and write miss handling
- Edge cases and error conditions

Run tests with:
```bash
make test
```

## Contributing

1. Follow the existing code style
2. Add tests for new features
3. Update documentation as needed
4. Ensure all tests pass before submitting

## License

This project is available for educational and research purposes.
