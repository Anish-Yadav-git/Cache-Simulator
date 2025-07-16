# Cache Simulator GUI Usage Guide

## Overview
The Cache Simulator provides two graphical user interfaces for simulating different cache architectures and analyzing their performance:

1. **Qt GUI** (`gui/qt/`) - Native desktop application with rich interface
2. **Web GUI** (`gui/web/`) - Browser-based interface with REST API backend

Both interfaces connect to the same C++ backend to perform actual cache simulations with real-time visualization.

## Features

### 1. Cache Configuration
- **Cache Size**: Configure total cache size (256B to 4KB)
- **Block Size**: Set cache block size (16B, 32B, 64B)
- **Associativity**: Choose between direct-mapped, set-associative, or fully associative
- **Replacement Policy**: Select from LRU, FIFO, or Random replacement algorithms
- **Write Policy**: Choose between Write-Through or Write-Back
- **Write Miss Policy**: Select Write-Allocate or No-Write-Allocate

### 2. Memory Access Simulation
- **Single Access**: Test individual memory address accesses
- **Batch Access**: Process multiple addresses from a list
- **Real-time Results**: See immediate HIT/MISS results
- **Operation Types**: Support for both READ and WRITE operations

### 3. Cache Visualization
- **Live Cache State**: Real-time display of cache contents
- **Color-coded Blocks**: Visual indication of valid/invalid and dirty/clean blocks
- **Set-by-Set View**: Organized display showing all cache sets and ways
- **Tag Information**: Display of stored tags for each valid block

### 4. Performance Statistics
- **Hit Rate**: Percentage of cache hits
- **Miss Rate**: Percentage of cache misses
- **Total Accesses**: Count of all memory accesses
- **Real-time Updates**: Statistics update with each access

### 5. Access Logging
- **Detailed Log**: Time-stamped record of all memory accesses
- **Export Feature**: Save access logs to text files
- **Clear Function**: Reset log for new experiments

## Getting Started

### Prerequisites
- Qt 6.x installed on your system
- C++ compiler with C++17 support
- CMake 3.14 or later

### Building the GUIs
```bash
# Navigate to project directory
cd "Cache Simulator"

# Build all components including both GUIs
make

# Run the Qt GUI (recommended)
./bin/cache_gui

# OR run the web GUI server
./bin/cache_server
# Then open browser to: gui/web/interface/index.html
```

## Choosing a GUI

### Qt GUI (Recommended)
The Qt GUI provides a rich native desktop application:
- **Launch**: `./bin/cache_gui`
- **Features**: Native widgets, better performance, integrated experience
- **Requirements**: Qt 6.x libraries

### Web GUI
The Web GUI provides a browser-based interface:
- **Launch**: `./bin/cache_server` then open `gui/web/interface/index.html`
- **Features**: 
  - Real-time cache visualization with color-coded blocks
  - Interactive cache configuration
  - Trace file upload and processing
  - Live statistics and performance monitoring
  - Cross-platform compatibility
  - RESTful API for integration
- **Requirements**: Modern web browser

## Using the GUI

### Step 1: Configure Cache
1. Select your desired cache parameters from the Configuration panel
2. Choose cache size, block size, and associativity
3. Select replacement and write policies
4. Click "Create Cache" to initialize the cache

### Step 2: Access Memory
1. Enter a memory address in hexadecimal format (e.g., 0x1000)
2. Choose READ or WRITE operation
3. Click "Access" to simulate the memory access
4. View the result (HIT/MISS) and updated statistics

### Step 3: Batch Processing
1. Click "Batch Access" to expand the batch input area
2. Enter multiple addresses, one per line
3. Click "Run Batch" to process all addresses
4. Monitor the access log for detailed results

### Step 4: Process Trace Files (Web GUI)
1. Click on "Trace File Processing" section
2. Either upload a trace file or paste trace content directly
3. Click "Process Trace" to execute all operations in the file
4. Monitor progress in real-time through the access log
5. View updated cache visualization and statistics

### Step 5: Analyze Results
1. Check the Statistics panel for performance metrics
2. Use the Cache Visualization to see current cache state
3. Review the Access Log for detailed operation history
4. Export logs for further analysis

## Cache Visualization Legend

### Color Coding
- **Light Green**: Valid, clean blocks
- **Light Red**: Valid, dirty blocks (write-back policy)
- **Light Gray**: Invalid blocks

### Block Information Format
- **V**: Valid bit (1 = valid, 0 = invalid)
- **D**: Dirty bit (1 = dirty, 0 = clean)
- **Tag**: Memory tag stored in the block (in hexadecimal)

### Example Display
```
V:1 D:0 Tag:0x1000  // Valid, clean block with tag 0x1000
V:1 D:1 Tag:0x2000  // Valid, dirty block with tag 0x2000
Invalid             // Invalid block
```

## Tips for Effective Use

### 1. Experiment with Different Configurations
- Try different associativities to see their impact on hit rates
- Compare LRU vs. FIFO replacement policies
- Test write-through vs. write-back policies

### 2. Use Batch Processing for Pattern Analysis
- Test sequential access patterns
- Analyze stride patterns
- Compare random vs. sequential access

### 3. Monitor Cache Behavior
- Watch how blocks are replaced
- Observe dirty bit changes with write operations
- Track tag changes as new blocks are loaded

### 4. Analyze Performance Metrics
- Higher associativity typically improves hit rates
- Write-back policy may show different dirty bit patterns
- LRU often performs better than FIFO for typical programs

## Troubleshooting

### Common Issues

1. **"Network Error" Messages**
   - Ensure the backend server is running
   - Check that the server is listening on port 8080
   - Verify network connectivity

2. **GUI Not Responding**
   - Restart the backend server
   - Check for any error messages in the server console
   - Ensure Qt libraries are properly installed

3. **Invalid Address Format**
   - Addresses must be in hexadecimal format (e.g., 0x1000)
   - Use the "0x" prefix for all addresses
   - Avoid spaces or special characters

### Performance Considerations
- Large cache sizes may take longer to visualize
- Batch processing of many addresses may cause temporary GUI freezing
- Use reasonable cache sizes for better responsiveness

## Advanced Features

### Custom Address Patterns
Create custom access patterns for specific testing scenarios:
- Sequential: 0x0, 0x20, 0x40, 0x60, ...
- Stride: 0x0, 0x100, 0x200, 0x300, ...
- Random: Generate random addresses within a range

### Export and Analysis
- Export access logs for external analysis
- Use log data for performance comparisons
- Generate reports for different cache configurations

## API Integration
The GUI communicates with the backend through REST API endpoints:
- `/create`: Create new cache configuration
- `/access`: Simulate memory access
- `/reset`: Reset cache state
- `/contents`: Get current cache contents
- `/info`: Get cache information and statistics

This allows for potential integration with other tools or automation scripts.
