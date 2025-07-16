# Web GUI Troubleshooting Guide

## Issue: "Error loading cache information"

This error occurs when the web interface cannot connect to the backend server. Here are the steps to resolve it:

### ✅ **Solution Steps:**

#### 1. **Start the Backend Server**
The web interface requires the backend server to be running:

```bash
cd "Cache Simulator"
make
./bin/cache_server
```

You should see output like:
```
Cache Simulator Server starting on port 8080
API Endpoints:
  GET /create?cache_size=1024&block_size=32&associativity=4&replacement_policy=LRU
  GET /access?address=0x1000&operation=READ
  GET /reset
  GET /info
  GET /contents
  GET /trace?trace_data=<trace_content>

Server running... (Press Ctrl+C to stop)
Server listening on port 8080
```

#### 2. **Open the Web Interface**
Open your web browser and navigate to:
```
file:///path/to/Cache Simulator/gui/web/interface/index.html
```

Replace `/path/to/` with the actual path to your Cache Simulator directory.

#### 3. **Test the Connection**
1. Open the browser's Developer Tools (F12)
2. Go to the Console tab
3. Create a cache configuration and click "Create Cache"
4. Check the console for any error messages

#### 4. **Verify Server Connection**
You can test the server manually:
```bash
# Test server connection
curl "http://localhost:8080/info"

# Create a cache
curl "http://localhost:8080/create?cache_size=1024&block_size=32&associativity=4&replacement_policy=LRU"

# Get cache contents
curl "http://localhost:8080/contents"
```

### 🔧 **If Server is Not Starting:**

1. **Check Port Availability:**
   ```bash
   lsof -i :8080
   ```
   If port 8080 is busy, start the server on a different port:
   ```bash
   ./bin/cache_server 8081
   ```
   Then update the `API_BASE` in `gui/web/interface/script.js` to `http://localhost:8081`

2. **Check Build Issues:**
   ```bash
   make clean && make
   ```

3. **Platform-specific Issues:**
   - **macOS**: Ensure Xcode command line tools are installed
   - **Linux**: Install build-essential and cmake
   - **Windows**: Use MinGW or Visual Studio

### 🌐 **If Server is Running but Web Interface Shows Error:**

1. **CORS Issues:**
   The server includes CORS headers, but some browsers may block local file access. Try:
   - Using a local web server (Python: `python -m http.server 8000`)
   - Or use the test page: `gui/web/interface/test.html`

2. **JavaScript Console Errors:**
   Check the browser console for specific error messages and ensure:
   - The `API_BASE` URL is correct in `script.js`
   - No network connectivity issues
   - No firewall blocking the connection

### 🧪 **Testing Different Scenarios:**

#### Test 1: Manual API Testing
```bash
# Start server
./bin/cache_server

# In another terminal:
curl "http://localhost:8080/create?cache_size=1024&block_size=32&associativity=4&replacement_policy=LRU"
curl "http://localhost:8080/access?address=0x1000&operation=read"
curl "http://localhost:8080/contents"
```

#### Test 2: Use the Test Page
Open `gui/web/interface/test.html` in your browser and click through each test button.

#### Test 3: Check Browser Network Tab
1. Open browser Developer Tools
2. Go to Network tab
3. Try creating a cache
4. Check if HTTP requests are being made to `localhost:8080`

### 📝 **Fallback Mode:**

If the server is not available, the web interface will automatically switch to simulation mode. In this mode:
- Cache visualization will show sample data
- Operations will be simulated with random results
- You'll see console messages indicating "Using simulated response"

### 🔍 **Debug Information:**

The web interface includes detailed logging. Open the browser console to see:
- API call URLs
- Server responses
- Cache visualization data
- Error messages

### 🆘 **Still Having Issues?**

1. **Run the automated test:**
   ```bash
   ./test_web_gui.sh
   ```

2. **Check server logs:**
   The server prints detailed information about incoming requests

3. **Verify system requirements:**
   - C++17 compatible compiler
   - CMake 3.14+
   - Modern web browser
   - Network connectivity

### 🎯 **Common Solutions:**

| Issue | Solution |
|-------|----------|
| Port 8080 in use | Use different port: `./bin/cache_server 8081` |
| CORS errors | Use local web server or test page |
| Build failures | Run `make clean && make` |
| Server not found | Check if server is running: `ps aux | grep cache_server` |
| Browser errors | Check console for specific error messages |

### 📊 **Expected Behavior:**

When working correctly, you should see:
1. **Cache Configuration:** Info panel showing cache parameters
2. **Cache Visualization:** Color-coded blocks showing cache state
3. **Real-time Updates:** Visualization updates after each operation
4. **Statistics:** Live hit/miss rates and access counts
5. **Access Log:** Detailed record of all operations

The cache visualization uses:
- **Green blocks**: Valid, clean cache blocks
- **Red blocks**: Valid, dirty cache blocks
- **Gray blocks**: Invalid cache blocks
- **Block details**: Valid/dirty bits and tag values
