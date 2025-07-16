# Qt GUI - Cache Simulator

This directory contains the Qt-based graphical user interface for the Cache Simulator.

## Files

- `cache_gui.h` - Header file for the Qt GUI main window class
- `cache_gui.cpp` - Implementation of the Qt GUI main window
- `main_gui.cpp` - Main entry point for the Qt GUI application

## Prerequisites

- Qt 6.x (Core, Widgets, Network modules)
- C++17 compatible compiler
- CMake 3.14 or higher

## Building

From the project root directory:

```bash
# Build the Qt GUI application
make

# Run the Qt GUI
./bin/cache_gui
```

## Features

- **Interactive Cache Configuration**: Configure cache size, block size, associativity, and policies
- **Memory Access Simulation**: Perform single or batch memory accesses
- **Real-time Visualization**: View cache contents and statistics in real-time
- **Statistics Display**: Monitor hit rates, miss rates, and access patterns
- **Access Log**: Track all memory accesses with detailed logging
- **Export Functionality**: Export logs and statistics for analysis

## Usage

1. Start the application: `./bin/cache_gui`
2. Configure your cache parameters in the "Cache Configuration" panel
3. Click "Create Cache" to initialize the cache
4. Use the "Memory Access" panel to perform cache operations
5. Monitor the cache visualization and statistics in real-time

## Architecture

The Qt GUI communicates with the backend server (`cache_server`) via HTTP requests to provide a seamless user experience with real-time cache simulation and visualization.
