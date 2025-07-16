#!/bin/bash

# Test script for web GUI functionality
echo "Testing Cache Simulator Web GUI..."
echo "================================="

# Change to project directory
cd "$(dirname "$0")"

# Build the project
echo "Building project..."
make clean && make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Start the server in background
echo "Starting server..."
./bin/cache_server &
SERVER_PID=$!

# Wait for server to start
sleep 2

# Test API endpoints
echo "Testing API endpoints..."

# Test 1: Create cache
echo "1. Testing cache creation..."
RESPONSE=$(curl -s "http://localhost:8080/create?cache_size=1024&block_size=32&associativity=4&replacement_policy=LRU")
echo "Response: $RESPONSE"

if [[ $RESPONSE == *"success"* ]]; then
    echo "✓ Cache creation successful"
else
    echo "✗ Cache creation failed"
fi

# Test 2: Get cache contents
echo "2. Testing cache contents retrieval..."
RESPONSE=$(curl -s "http://localhost:8080/contents")
echo "Response: ${RESPONSE:0:100}..."

if [[ $RESPONSE == *"success"* ]]; then
    echo "✓ Cache contents retrieval successful"
else
    echo "✗ Cache contents retrieval failed"
fi

# Test 3: Memory access
echo "3. Testing memory access..."
RESPONSE=$(curl -s "http://localhost:8080/access?address=0x1000&operation=read")
echo "Response: $RESPONSE"

if [[ $RESPONSE == *"success"* ]]; then
    echo "✓ Memory access successful"
else
    echo "✗ Memory access failed"
fi

# Test 4: Cache contents after access
echo "4. Testing cache contents after access..."
RESPONSE=$(curl -s "http://localhost:8080/contents")
echo "Response: ${RESPONSE:0:200}..."

if [[ $RESPONSE == *"success"* ]] && [[ $RESPONSE == *"valid\": true"* ]]; then
    echo "✓ Cache visualization working correctly"
else
    echo "✗ Cache visualization issue detected"
fi

# Test 5: Reset cache
echo "5. Testing cache reset..."
RESPONSE=$(curl -s "http://localhost:8080/reset")
echo "Response: $RESPONSE"

if [[ $RESPONSE == *"success"* ]]; then
    echo "✓ Cache reset successful"
else
    echo "✗ Cache reset failed"
fi

# Clean up
echo "Cleaning up..."
kill $SERVER_PID 2>/dev/null

echo "================================="
echo "Test completed!"
echo ""
echo "To test the web interface:"
echo "1. Start the server: ./bin/cache_server"
echo "2. Open browser to: file://$(pwd)/gui/web/interface/index.html"
echo "3. Or use the test page: file://$(pwd)/gui/web/interface/test.html"
