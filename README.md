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
- **📋 Batch Processing**: Support for custom memory access patterns and trace files
- **📄 File I/O**: Read trace files and write detailed statistics reports

## 🚀 Quick Start

### Build and Run
```bash
# Build the project
make

# Run with default configuration
./bin/cache_simulator

# Show help
./bin/cache_simulator --help
```

### Example Usage
```bash
# Process trace file
./bin/cache_simulator -t examples/traces/sample_trace.txt -o examples/output/results.txt

# Custom cache configuration
./bin/cache_simulator -s 2048 -b 64 -a 8 -r LRU -w WRITE_BACK

# Interactive mode
./bin/cache_simulator --interactive
```

## 📁 Project Structure

```
cache-simulator/
├── 📁 docs/                    # Documentation
├── 📁 examples/                # Example files and traces
│   ├── traces/                 # Sample trace files
│   └── output/                 # Output files (git-ignored)
├── 📁 include/                 # Header files
├── 📁 src/                     # Source files
├── 📁 tests/                   # Test files
├── 📁 gui/                     # GUI implementation
├── 📁 build/                   # Build artifacts (git-ignored)
├── 📁 bin/                     # Executables (git-ignored)
├── CMakeLists.txt              # CMake configuration
├── Makefile                    # Build wrapper
├── .gitignore                  # Git ignore rules
└── LICENSE                     # MIT License
```

## 🛠️ Building

### Prerequisites
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.14** or higher
- **Make** utility

### Build Commands
```bash
# Quick build and run
make run

# Build all targets
make

# Clean build
make clean && make

# Run tests
make test

# Build debug version
make debug
```

## 📖 Documentation

- **[Complete User Guide](docs/README.md)** - Comprehensive documentation
- **[Trace File Guide](docs/TRACE_FILE_GUIDE.md)** - How to use trace files
- **[Demo Guide](docs/DEMONSTRATION_GUIDE.md)** - Functionality verification

## 🧪 Testing

```bash
# Run comprehensive tests
make test

# Run functionality verification
./tests/test_comprehensive.sh
```

## 🔧 Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `-s, --cache-size` | Cache size in bytes | 1024 |
| `-b, --block-size` | Block size in bytes | 32 |
| `-a, --associativity` | Associativity (1=direct, 0=fully) | 4 |
| `-r, --replacement` | Replacement policy (LRU/FIFO/RANDOM) | LRU |
| `-w, --write-policy` | Write policy (WRITE_THROUGH/WRITE_BACK) | WRITE_THROUGH |
| `-t, --trace-file` | Input trace file | - |
| `-o, --output-file` | Output statistics file | stats.txt |
| `-i, --interactive` | Interactive mode | false |
| `-v, --verbose` | Verbose output | false |
| `-q, --quiet` | Suppress console output | false |

## 📊 Example Output

```
Cache Statistics:
  Total Accesses: 36
  Hits: 32
  Misses: 4
  Hit Rate: 88.89%
  Miss Rate: 11.11%

  Read Accesses: 22
  Write Accesses: 14
  Read Hit Rate: 90.91%
  Write Hit Rate: 85.71%
```

## 🎯 Use Cases

- **Academic Research**: Analyze real program traces and cache behavior
- **Education**: Learn computer architecture concepts with hands-on simulation
- **Performance Analysis**: Evaluate cache configurations for specific workloads
- **Algorithm Development**: Test and compare replacement policies

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Computer Architecture textbooks and research papers
- Open-source cache simulation tools
- Educational institutions using this for teaching

---

**Made with ❤️ for computer architecture education and research**
