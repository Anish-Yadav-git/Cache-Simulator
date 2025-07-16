# GUI - Cache Simulator

This directory contains the graphical user interface implementations for the Cache Simulator project.

## Directory Structure

```
gui/
├── qt/                          # Qt-based GUI application
│   ├── cache_gui.cpp            # Qt GUI implementation
│   ├── cache_gui.h              # Qt GUI header
│   ├── main_gui.cpp             # Qt GUI main entry point
│   └── README.md                # Qt GUI documentation
├── web/                         # Web-based GUI
│   ├── cache_server.cpp         # HTTP server backend
│   ├── interface/               # Web interface files
│   │   ├── index.html           # Main web page
│   │   ├── script.js            # JavaScript functionality
│   │   └── style.css            # CSS styling
│   └── README.md                # Web GUI documentation
└── README.md                    # This file
```

## GUI Options

### 1. Qt GUI (Recommended)

The Qt-based GUI provides a native desktop application with rich user interface components.

**Features:**
- Native desktop application
- Real-time cache visualization
- Interactive configuration panels
- Comprehensive statistics display
- Export functionality

**Usage:**
```bash
# Build and run Qt GUI
make
./bin/cache_gui
```

### 2. Web GUI

The web-based GUI provides a browser-based interface that's platform-independent.

**Features:**
- Browser-based interface
- RESTful API backend
- Cross-platform compatibility
- Real-time updates via HTTP requests
- No additional software installation required

**Usage:**
```bash
# Start the web server
./bin/cache_server

# Open browser to: gui/web/interface/index.html
```

## Choosing the Right GUI

| Feature | Qt GUI | Web GUI |
|---------|--------|---------|
| Installation | Requires Qt libraries | Just web browser |
| Performance | Native, fast | Network overhead |
| User Experience | Rich desktop UI | Standard web UI |
| Customization | Full Qt capabilities | HTML/CSS/JS |
| Integration | Standalone app | Can integrate with web tools |
| Platform Support | Qt-supported platforms | Any modern browser |

## Building

Both GUI applications are built with the main project:

```bash
# Build all components including GUIs
make

# Build individual targets
make cache_gui      # Qt GUI
make cache_server   # Web GUI backend
```

## Development

### Qt GUI Development

The Qt GUI is implemented using Qt 6.x with the following components:
- **Qt Core**: Basic Qt functionality
- **Qt Widgets**: GUI components
- **Qt Network**: HTTP communication with backend

### Web GUI Development

The web GUI consists of:
- **Backend**: C++ HTTP server (`cache_server.cpp`)
- **Frontend**: HTML/CSS/JavaScript (`interface/` directory)
- **Communication**: REST API with JSON data exchange

## API Documentation

Both GUIs interact with the same cache simulation backend. The web GUI exposes this as a REST API:

### Cache Operations
- `POST /create` - Create new cache with specified parameters
- `GET /access` - Perform memory access operation
- `GET /reset` - Reset cache to empty state
- `GET /info` - Get cache configuration and statistics
- `GET /contents` - Get detailed cache contents

### Parameters
- `cache_size` - Cache size in bytes
- `block_size` - Block size in bytes
- `associativity` - Cache associativity (1=direct, 0=fully associative)
- `replacement_policy` - LRU, FIFO, or RANDOM
- `write_policy` - WRITE_THROUGH or WRITE_BACK
- `write_miss_policy` - WRITE_ALLOCATE or NO_WRITE_ALLOCATE

## Contributing

When adding new GUI features:

1. **Qt GUI**: Modify files in `gui/qt/` directory
2. **Web GUI**: Update backend (`cache_server.cpp`) and/or frontend (`interface/`)
3. **Documentation**: Update relevant README files
4. **Build System**: Update `CMakeLists.txt` if needed

## Troubleshooting

### Qt GUI Issues
- Ensure Qt 6.x is installed
- Check CMake can find Qt packages
- Verify C++17 compiler support

### Web GUI Issues
- Ensure backend server is running
- Check network connectivity (localhost:8080)
- Verify CORS settings for cross-origin requests
- Check browser console for JavaScript errors

## Future Enhancements

- **Mobile Support**: Responsive web interface for mobile devices
- **Advanced Visualization**: 3D cache visualization
- **Performance Monitoring**: Real-time performance metrics
- **Collaboration Features**: Multi-user cache simulation
- **Plugin System**: Extensible GUI architecture
