# Cache Simulator Installation Guide

## System Requirements

### Operating Systems
- macOS 12.0 or later
- Linux (Ubuntu 20.04 LTS or later)
- Windows 10/11 (with MinGW or Visual Studio)

### Dependencies
- **C++ Compiler**: GCC 9.0+, Clang 10.0+, or MSVC 2019+
- **CMake**: Version 3.14 or later
- **Qt**: Version 6.0 or later
- **Network Support**: For GUI-backend communication

## Installation Instructions

### macOS

#### 1. Install Prerequisites
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake
brew install cmake

# Install Qt 6
brew install qt

# Verify installations
cmake --version
qmake --version
```

#### 2. Clone and Build
```bash
# Clone the repository
git clone <repository-url>
cd "Cache Simulator"

# Create build directory
mkdir -p build && cd build

# Configure with CMake
cmake ..

# Build all targets
make

# Verify build
ls ../bin/
```

#### 3. Run the Applications
```bash
# Option 1: Qt GUI (Recommended)
./bin/cache_gui

# Option 2: Web GUI
# Start backend server (in one terminal)
./bin/cache_server
# Then open browser to: gui/web/interface/index.html

# Option 3: Command-line version
./bin/cache_simulator
```

### Linux (Ubuntu/Debian)

#### 1. Install Prerequisites
```bash
# Update package list
sudo apt update

# Install build tools
sudo apt install build-essential cmake

# Install Qt 6
sudo apt install qt6-base-dev qt6-tools-dev libqt6network6

# For older Ubuntu versions, you might need:
# sudo apt install qtbase5-dev qttools5-dev libqt5network5-dev
```

#### 2. Build Project
```bash
# Clone repository
git clone <repository-url>
cd "Cache Simulator"

# Build
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

#### 3. Run Applications
```bash
# Make sure binaries are executable
chmod +x ../bin/*

# Option 1: Qt GUI (Recommended)
./bin/cache_gui

# Option 2: Web GUI
# Start backend server
./bin/cache_server &
# Then open browser to: gui/web/interface/index.html

# Option 3: Command-line version
./bin/cache_simulator
```

### Windows

#### Option 1: Using Visual Studio

1. **Install Visual Studio 2019 or later**
   - Include C++ development tools
   - Install CMake tools

2. **Install Qt 6**
   - Download Qt installer from qt.io
   - Install Qt 6.x with MSVC compiler support

3. **Build Project**
   ```cmd
   # Open Command Prompt or PowerShell
   git clone <repository-url>
   cd "Cache Simulator"
   
   mkdir build
   cd build
   
   # Configure (adjust Qt path as needed)
   cmake -DCMAKE_PREFIX_PATH="C:\Qt\6.x\msvc2019_64" ..
   
   # Build
   cmake --build . --config Release
   ```

#### Option 2: Using MinGW

1. **Install MinGW-w64**
2. **Install Qt 6 with MinGW support**
3. **Build similar to Linux instructions**

## Configuration

### Environment Variables
You may need to set these environment variables:

```bash
# For Qt (adjust path as needed)
export PATH="/opt/homebrew/bin:$PATH"  # macOS
export PATH="/usr/lib/qt6/bin:$PATH"   # Linux

# For CMake to find Qt
export CMAKE_PREFIX_PATH="/opt/homebrew"  # macOS
export CMAKE_PREFIX_PATH="/usr/lib/qt6"   # Linux
```

### Network Configuration
The GUI connects to the backend server on `localhost:8080` by default. To change this:

1. **Server Port**: Modify port in `gui/web/cache_server.cpp`
2. **Qt GUI Client URL**: Update `m_baseURL` in `gui/qt/cache_gui.cpp`
3. **Web GUI Client**: Update server URL in `gui/web/interface/script.js`

## Verification

### Test Installation
```bash
# Test command-line version
./bin/cache_simulator --help

# Test with a simple configuration
./bin/cache_simulator --cache-size 1024 --block-size 32 --associativity 4

# Test server
./bin/cache_server &
curl "http://localhost:8080/create?cache_size=1024&block_size=32&associativity=4&replacement_policy=LRU"
```

### GUI Testing
1. Start the backend server
2. Launch the GUI
3. Create a cache configuration
4. Perform a memory access
5. Verify the visualization updates

## Troubleshooting

### Common Build Issues

#### Qt Not Found
```bash
# Error: "Could not find Qt6"
# Solution: Install Qt 6 and set CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH="/path/to/qt6"
cmake ..
```

#### CMake Version Too Old
```bash
# Error: "CMake 3.14 or higher is required"
# Solution: Update CMake
brew install cmake  # macOS
sudo apt install cmake  # Ubuntu
```

#### Compiler Not Found
```bash
# Error: "No C++ compiler found"
# Solution: Install compiler
sudo apt install build-essential  # Ubuntu
xcode-select --install  # macOS
```

### Runtime Issues

#### Server Connection Failed
1. Ensure backend server is running
2. Check port availability: `netstat -an | grep 8080`
3. Verify firewall settings

#### GUI Not Starting
1. Check Qt installation: `qmake --version`
2. Verify Qt libraries are in PATH
3. On Linux, may need: `export QT_QPA_PLATFORM=xcb`

#### Permission Denied
```bash
# Fix executable permissions
chmod +x bin/*
```

## Advanced Configuration

### Custom Build Options
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Custom installation prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

### Development Setup
```bash
# Enable additional warnings
cmake -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic" ..

# Enable sanitizers (debug builds)
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address -fsanitize=undefined" ..
```

## Packaging

### Create Distribution Package
```bash
# Install to staging directory
make install DESTDIR=/tmp/cache-simulator-package

# Create tarball
cd /tmp
tar -czf cache-simulator-1.0.tar.gz cache-simulator-package/

# Or create platform-specific packages
cpack  # If CPack is configured
```

### Docker Container
```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-tools-dev \
    libqt6network6

COPY . /app
WORKDIR /app

RUN mkdir build && cd build && \
    cmake .. && \
    make

EXPOSE 8080
CMD ["./bin/cache_server"]
```

## Support

For installation issues:
1. Check system requirements
2. Review error messages carefully
3. Ensure all dependencies are installed
4. Try building in a clean environment
5. Consult platform-specific documentation

For more help, refer to:
- CMake documentation
- Qt installation guides
- Platform-specific build instructions
