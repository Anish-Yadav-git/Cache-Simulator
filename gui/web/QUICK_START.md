# Web GUI Quick Start Guide

## 🚀 Getting Started

### 1. Start the Backend Server
```bash
cd "Cache Simulator"
make
./bin/cache_server
```

### 2. Open the Web Interface
Open your web browser and navigate to:
```
file:///path/to/Cache Simulator/gui/web/interface/index.html
```

## 🎯 Basic Usage

### Step 1: Configure Cache
1. Select cache parameters:
   - **Cache Size**: 1024 bytes (default)
   - **Block Size**: 32 bytes (default)
   - **Associativity**: 4-way (default)
   - **Replacement Policy**: LRU (default)
   - **Write Policy**: Write-Through (default)

2. Click **"Create Cache"** to initialize

### Step 2: Test Memory Access
1. Enter a memory address: `0x1000`
2. Select operation: `READ`
3. Click **"Single Access"**
4. Watch the cache visualization update!

### Step 3: Try Batch Operations
1. Click **"Batch Access"** to expand the panel
2. Enter multiple addresses:
   ```
   0x1000
   0x2000
   0x3000
   ```
3. Click **"Run Batch"**
4. Monitor the access log

### Step 4: Process Trace Files
1. Use the provided test trace file: `examples/traces/web_test.txt`
2. Either upload the file or copy its contents
3. Click **"Process Trace"**
4. Watch the cache visualization change in real-time

## 🔍 Understanding the Interface

### Cache Visualization
- **Green blocks**: Valid, clean cache blocks
- **Red blocks**: Valid, dirty cache blocks (write-back policy)
- **Gray blocks**: Invalid cache blocks
- **Block info**: Shows valid/dirty bits and tag values

### Statistics Panel
- **Real-time updates** after each cache operation
- **Hit/Miss rates** with percentage calculations
- **Color-coded statistics** for easy reading

### Access Log
- **Chronological record** of all memory operations
- **Color-coded results**: Green for hits, red for misses
- **Export functionality** for further analysis

## 📁 Trace File Format

Create your own trace files with this format:
```
R 0x1000    # Read from address 0x1000
W 0x2000    # Write to address 0x2000
R 0x1000    # Read from address 0x1000 (likely a hit)
```

## 🛠️ Troubleshooting

### Server Not Starting
- Check if port 8080 is available
- Try running: `./bin/cache_server 8081` for different port

### Web Interface Not Loading
- Ensure server is running first
- Check browser console for errors
- Try refreshing the page

### Cache Not Updating
- Verify cache was created successfully
- Check server console for error messages
- Ensure addresses are in hex format (0x prefix)

## 🎨 Advanced Features

### REST API Testing
Test the API directly:
```bash
# Create cache
curl "http://localhost:8080/create?cache_size=1024&block_size=32&associativity=4&replacement_policy=LRU"

# Access memory
curl "http://localhost:8080/access?address=0x1000&operation=READ"

# Get cache contents
curl "http://localhost:8080/contents"
```

### Custom Configurations
Experiment with different cache configurations:
- **Direct-mapped**: Associativity = 1
- **Fully associative**: Associativity = 0
- **Write-back policy**: For dirty bit visualization
- **Different sizes**: See how cache size affects performance

## 📊 Performance Analysis

Use the web interface to:
1. **Compare policies**: Test LRU vs FIFO vs Random
2. **Analyze patterns**: Sequential vs random access
3. **Study write behavior**: Write-through vs write-back
4. **Measure hit rates**: Different cache sizes and associativities

## 🚀 Next Steps

1. Try the Qt GUI for comparison: `./bin/cache_gui`
2. Explore the command-line interface: `./bin/cache_simulator --help`
3. Read the comprehensive documentation in `docs/`
4. Create your own trace files for specific test scenarios

Happy cache simulating! 🎉
