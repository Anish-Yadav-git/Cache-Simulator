# Web GUI - Cache Simulator

This directory contains the web-based graphical user interface for the Cache Simulator.

## Files

- `cache_server.cpp` - Backend HTTP server for the web interface
- `interface/` - Directory containing web interface files
  - `index.html` - Main web page
  - `script.js` - JavaScript for web interface functionality
  - `style.css` - CSS styling for the web interface

## Prerequisites

- C++17 compatible compiler
- Standard networking libraries
- Modern web browser

## Building

From the project root directory:

```bash
# Build the web server
make

# Run the web server
./bin/cache_server
```

## Usage

### Starting the Web Interface

1. **Start the backend server:**
   ```bash
   ./bin/cache_server
   ```
   The server will start on port 8080 by default.

2. **Open the web interface:**
   Open your web browser and navigate to:
   ```
   file:///path/to/Cache Simulator/gui/web/interface/index.html
   ```
   Or serve the files through a local web server.

### API Endpoints

The web server provides the following REST API endpoints:

- `GET /create?cache_size=1024&block_size=32&associativity=4&replacement_policy=LRU` - Create a new cache
- `GET /access?address=0x1000&operation=READ` - Access cache memory
- `GET /reset` - Reset cache to empty state
- `GET /info` - Get cache configuration and statistics
- `GET /contents` - Get detailed cache contents with visualization data
- `GET /trace?trace_data=<encoded_trace_content>` - Process trace file operations

### Web Interface Features

- **Browser-based GUI**: No additional software installation required
- **Real-time Cache Visualization**: Live display of cache contents with color-coded blocks
- **Interactive Configuration**: Configure cache parameters through web interface
- **Memory Access Simulation**: Single and batch memory access operations
- **Trace File Support**: Upload and process trace files for automated testing
- **Live Statistics**: Real-time hit/miss rates and performance metrics
- **RESTful API**: Clean API for integration with other tools
- **Cross-platform**: Works on any system with a web browser
- **Responsive Design**: Adapts to different screen sizes

## Architecture

The web interface follows a client-server architecture:

1. **Backend Server** (`cache_server.cpp`): Handles cache operations and provides HTTP API
2. **Frontend** (`interface/`): HTML/CSS/JavaScript web interface that communicates with the backend
3. **Communication**: HTTP requests/responses with JSON data format

## Using the Web Interface

### Basic Usage

1. **Configure Cache**: Select cache parameters (size, block size, associativity, policies)
2. **Create Cache**: Click "Create Cache" to initialize with selected parameters
3. **Access Memory**: Enter memory addresses and perform READ/WRITE operations
4. **View Results**: Monitor cache visualization and statistics in real-time

### Cache Visualization

The web interface provides live cache visualization with:
- **Color-coded blocks**: 
  - Green: Valid, clean blocks
  - Red: Valid, dirty blocks (write-back policy)
  - Gray: Invalid blocks
- **Block information**: Shows valid/dirty bits and tag values
- **Set organization**: Displays cache sets with their associated blocks

### Trace File Processing

1. **Upload File**: Select a trace file using the file input
2. **Manual Input**: Or paste trace content directly into the text area
3. **Process Trace**: Click "Process Trace" to execute all operations
4. **View Results**: Monitor progress in access log and see updated statistics

#### Trace File Format

Trace files should contain one operation per line:
```
R 0x1000    # Read from address 0x1000
W 0x2000    # Write to address 0x2000
R 0x1000    # Read from address 0x1000
```

### Batch Operations

1. Click "Batch Access" to expand the batch panel
2. Enter multiple addresses (one per line)
3. Click "Run Batch" to process all addresses sequentially
4. Monitor results in the access log

### Statistics and Monitoring

- **Live Updates**: Statistics update after each cache operation
- **Hit/Miss Rates**: Real-time calculation of performance metrics
- **Access Log**: Detailed log of all memory operations with timestamps
- **Export**: Save access logs for further analysis

## Development

To modify the web interface:

1. Edit the HTML/CSS/JavaScript files in the `interface/` directory
2. Restart the backend server if needed
3. Refresh the browser to see changes

## Network Configuration

The default configuration uses:
- **Port**: 8080
- **Host**: localhost
- **CORS**: Enabled for cross-origin requests

## Troubleshooting

### Common Issue: "Error loading cache information"

This error occurs when the web interface cannot connect to the backend server.

**Solution:**
1. **Start the backend server first:**
   ```bash
   ./bin/cache_server
   ```

2. **Then open the web interface:**
   ```
   file:///path/to/Cache Simulator/gui/web/interface/index.html
   ```

3. **Test the connection:**
   - Open browser Developer Tools (F12)
   - Check Console tab for error messages
   - Verify server is running: `curl http://localhost:8080/info`

### Other Issues
- **Port 8080 in use**: Use different port: `./bin/cache_server 8081`
- **CORS errors**: Use local web server or test page
- **Build failures**: Run `make clean && make`
- **Browser errors**: Check console for specific error messages

### Testing
- **Use test page**: `gui/web/interface/test.html`
- **Run automated test**: `./test_web_gui.sh`
- **Manual API testing**: See `gui/web/TROUBLESHOOTING.md`

## Security Note

This server is designed for educational and development purposes. For production use, additional security measures should be implemented.
