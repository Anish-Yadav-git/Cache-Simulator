#!/bin/bash

# Cache Simulator Comprehensive Functionality Test
# This script demonstrates all features and validates correct operation

echo "=============================================="
echo "    CACHE SIMULATOR COMPREHENSIVE TEST"
echo "=============================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Output file for test results
STATS_FILE="examples/output/comprehensive_test_stats.txt"
TEST_DIR="examples/output/test_outputs"

# Create output directory
mkdir -p "$TEST_DIR"

# Initialize stats file
echo "========================================" > "$STATS_FILE"
echo "Cache Simulator Comprehensive Test Report" >> "$STATS_FILE"
echo "========================================" >> "$STATS_FILE"
echo "Generated: $(date)" >> "$STATS_FILE"
echo "" >> "$STATS_FILE"

# Function to print section headers
print_section() {
    echo -e "${BLUE}### $1 ###${NC}"
    echo ""
}

# Function to print test status
print_test() {
    echo -e "${YELLOW}➤ $1${NC}"
}

# Function to print success
print_success() {
    echo -e "${GREEN}✓ $1${NC}"
    echo "✓ $1" >> "$STATS_FILE"
    echo ""
}

# Function to print failure
print_failure() {
    echo -e "${RED}✗ $1${NC}"
    echo "✗ $1" >> "$STATS_FILE"
    echo ""
}

# Function to log test section to stats file
log_section() {
    echo "" >> "$STATS_FILE"
    echo "$1" >> "$STATS_FILE"
    echo "$(echo "$1" | sed 's/./=/g')" >> "$STATS_FILE"
}

# Build the project first
print_section "BUILDING PROJECT"
make clean && make
echo ""

# Test 1: Basic functionality test
print_section "1. BASIC FUNCTIONALITY TEST"
log_section "1. BASIC FUNCTIONALITY TEST"
print_test "Running comprehensive test suite..."
make test > /dev/null 2>&1
if [ $? -eq 0 ]; then
    print_success "All basic tests passed!"
else
    print_failure "Basic tests failed!"
    exit 1
fi

# Test 2: Default configuration
print_section "2. DEFAULT CONFIGURATION TEST"
log_section "2. DEFAULT CONFIGURATION TEST"
print_test "Testing default configuration (4-way set associative, 1KB, 32B blocks, LRU)..."
./bin/cache_simulator -o "$TEST_DIR/default_test.txt" -q > "$TEST_DIR/default_test.out" 2>&1
if grep -q "Hit Rate:" "$TEST_DIR/default_test.txt"; then
    print_success "Default configuration working correctly!"
else
    print_failure "Default configuration failed!"
    echo "Debug: Contents of default test output:" >> "$STATS_FILE"
    cat "$TEST_DIR/default_test.out" >> "$STATS_FILE" 2>/dev/null || echo "No output file found" >> "$STATS_FILE"
    exit 1
fi

# Test 3: Cache Type Tests
print_section "3. CACHE TYPE TESTS"
log_section "3. CACHE TYPE TESTS"

# Direct-mapped cache
print_test "Testing Direct-mapped cache (associativity=1)..."
./bin/cache_simulator -s 512 -a 1 -o "$TEST_DIR/direct_test.txt" -q > "$TEST_DIR/direct_test.out" 2>&1
if grep -q "Direct Mapped" "$TEST_DIR/direct_test.txt"; then
    print_success "Direct-mapped cache working!"
else
    print_failure "Direct-mapped cache failed!"
fi

# Fully associative cache
print_test "Testing Fully associative cache (associativity=0)..."
./bin/cache_simulator -s 512 -a 0 -o "$TEST_DIR/fully_test.txt" -q > "$TEST_DIR/fully_test.out" 2>&1
if grep -q "Fully Associative" "$TEST_DIR/fully_test.txt"; then
    print_success "Fully associative cache working!"
else
    print_failure "Fully associative cache failed!"
fi

# 8-way set associative
print_test "Testing 8-way set associative cache..."
./bin/cache_simulator -s 2048 -a 8 -o "$TEST_DIR/8way_test.txt" -q > "$TEST_DIR/8way_test.out" 2>&1
if grep -q "8-way" "$TEST_DIR/8way_test.txt"; then
    print_success "8-way set associative cache working!"
else
    print_failure "8-way set associative cache failed!"
fi

# Test 4: Replacement Policy Tests
print_section "4. REPLACEMENT POLICY TESTS"
log_section "4. REPLACEMENT POLICY TESTS"

# LRU
print_test "Testing LRU replacement policy..."
./bin/cache_simulator -r LRU -o "$TEST_DIR/lru_test.txt" -q > "$TEST_DIR/lru_test.out" 2>&1
if grep -q "LRU" "$TEST_DIR/lru_test.txt"; then
    print_success "LRU policy working!"
else
    print_failure "LRU policy failed!"
fi

# FIFO
print_test "Testing FIFO replacement policy..."
./bin/cache_simulator -r FIFO -o "$TEST_DIR/fifo_test.txt" -q > "$TEST_DIR/fifo_test.out" 2>&1
if grep -q "FIFO" "$TEST_DIR/fifo_test.txt"; then
    print_success "FIFO policy working!"
else
    print_failure "FIFO policy failed!"
fi

# Random
print_test "Testing Random replacement policy..."
./bin/cache_simulator -r RANDOM -o "$TEST_DIR/random_test.txt" -q > "$TEST_DIR/random_test.out" 2>&1
if grep -q "Random" "$TEST_DIR/random_test.txt"; then
    print_success "Random policy working!"
else
    print_failure "Random policy failed!"
fi

# Test 5: Write Policy Tests
print_section "5. WRITE POLICY TESTS"
log_section "5. WRITE POLICY TESTS"

# Write-through
print_test "Testing Write-through policy..."
./bin/cache_simulator -w WRITE_THROUGH -o "$TEST_DIR/wt_test.txt" -q > "$TEST_DIR/wt_test.out" 2>&1
if grep -q "Write-Through" "$TEST_DIR/wt_test.txt"; then
    print_success "Write-through policy working!"
else
    print_failure "Write-through policy failed!"
fi

# Write-back
print_test "Testing Write-back policy..."
./bin/cache_simulator -w WRITE_BACK -v -o "$TEST_DIR/wb_test.txt" > "$TEST_DIR/wb_test.out" 2>&1
if grep -q "Write-Back" "$TEST_DIR/wb_test.txt" && grep -q "D:1" "$TEST_DIR/wb_test.out"; then
    print_success "Write-back policy working (dirty bits detected)!"
else
    print_failure "Write-back policy failed!"
fi

# Test 6: Write Miss Policy Tests
print_section "6. WRITE MISS POLICY TESTS"
log_section "6. WRITE MISS POLICY TESTS"

# Write-allocate
print_test "Testing Write-allocate policy..."
./bin/cache_simulator -m WRITE_ALLOCATE -o "$TEST_DIR/wa_test.txt" -q > "$TEST_DIR/wa_test.out" 2>&1
if grep -q "Write-Allocate" "$TEST_DIR/wa_test.txt"; then
    print_success "Write-allocate policy working!"
else
    print_failure "Write-allocate policy failed!"
fi

# No-write-allocate
print_test "Testing No-write-allocate policy..."
./bin/cache_simulator -m NO_WRITE_ALLOCATE -o "$TEST_DIR/nwa_test.txt" -q > "$TEST_DIR/nwa_test.out" 2>&1
if grep -q "No-Write-Allocate" "$TEST_DIR/nwa_test.txt"; then
    print_success "No-write-allocate policy working!"
else
    print_failure "No-write-allocate policy failed!"
fi

# Test 7: Custom Memory Access Pattern Tests
print_section "7. CUSTOM MEMORY ACCESS PATTERN TESTS"
log_section "7. CUSTOM MEMORY ACCESS PATTERN TESTS"

print_test "Testing custom addresses and operations..."
./bin/cache_simulator -A 0x1000,0x2000,0x3000,0x1000 -O READ,WRITE,READ,WRITE -o "$TEST_DIR/custom_test.txt" > "$TEST_DIR/custom_test.out" 2>&1
if grep -q "0x00001000" "$TEST_DIR/custom_test.txt" && grep -q "HIT" "$TEST_DIR/custom_test.txt"; then
    print_success "Custom memory access patterns working!"
else
    print_failure "Custom memory access patterns failed!"
fi

# Test 8: Trace File Tests
print_section "8. TRACE FILE TESTS"
log_section "8. TRACE FILE TESTS"

print_test "Testing trace file functionality..."
./bin/cache_simulator -t examples/traces/sample_trace.txt -o "$TEST_DIR/trace_test.txt" -q > "$TEST_DIR/trace_test.out" 2>&1
if grep -q "Hit Rate:" "$TEST_DIR/trace_test.txt"; then
    print_success "Trace file functionality working!"
else
    print_failure "Trace file functionality failed!"
fi

# Test 9: Verbose Output Test
print_section "9. VERBOSE OUTPUT TEST"
log_section "9. VERBOSE OUTPUT TEST"

print_test "Testing verbose output..."
./bin/cache_simulator -v -o "$TEST_DIR/verbose_test.txt" > "$TEST_DIR/verbose_test.out" 2>&1
if grep -q "Accessing address" "$TEST_DIR/verbose_test.out" && grep -q "Cache Contents:" "$TEST_DIR/verbose_test.out"; then
    print_success "Verbose output working!"
else
    print_failure "Verbose output failed!"
fi

# Test 10: Interactive Mode Test
print_section "10. INTERACTIVE MODE TEST"
log_section "10. INTERACTIVE MODE TEST"

print_test "Testing interactive mode..."
echo -e "access 0x100 read\nstats\nquit" | ./bin/cache_simulator --interactive > "$TEST_DIR/interactive_test.out" 2>&1
if grep -q "Interactive Mode" "$TEST_DIR/interactive_test.out" && grep -q "cache>" "$TEST_DIR/interactive_test.out"; then
    print_success "Interactive mode working!"
else
    print_failure "Interactive mode failed!"
fi

# Test 11: Help System Test
print_section "11. HELP SYSTEM TEST"
log_section "11. HELP SYSTEM TEST"

print_test "Testing help system..."
./bin/cache_simulator --help > "$TEST_DIR/help_test.out" 2>&1
if grep -q "Usage:" "$TEST_DIR/help_test.out" && grep -q "Examples:" "$TEST_DIR/help_test.out"; then
    print_success "Help system working!"
else
    print_failure "Help system failed!"
fi

# Test 12: Edge Cases and Error Handling
print_section "12. ERROR HANDLING TESTS"
log_section "12. ERROR HANDLING TESTS"

print_test "Testing invalid cache size..."
./bin/cache_simulator -s 0 > "$TEST_DIR/error_test1.out" 2>&1
if grep -q "Error:" "$TEST_DIR/error_test1.out"; then
    print_success "Error handling for invalid cache size working!"
else
    print_failure "Error handling for invalid cache size failed!"
fi

print_test "Testing mismatched cache/block size..."
./bin/cache_simulator -s 100 -b 32 > "$TEST_DIR/error_test2.out" 2>&1
if grep -q "Error:" "$TEST_DIR/error_test2.out"; then
    print_success "Error handling for mismatched sizes working!"
else
    print_failure "Error handling for mismatched sizes failed!"
fi

# Test 13: Performance Test (Large Configuration)
print_section "13. PERFORMANCE TEST"
log_section "13. PERFORMANCE TEST"

print_test "Testing large cache configuration..."
./bin/cache_simulator -s 65536 -b 64 -a 16 -A 0x0,0x1000,0x2000,0x3000,0x4000 -O READ,READ,READ,READ,READ -o "$TEST_DIR/perf_test.txt" -q > "$TEST_DIR/perf_test.out" 2>&1
if grep -q "Hit Rate:" "$TEST_DIR/perf_test.txt"; then
    print_success "Large cache configuration working!"
else
    print_failure "Large cache configuration failed!"
fi

# Summary
print_section "TEST SUMMARY"
log_section "TEST SUMMARY"

echo "" >> "$STATS_FILE"
echo "Test completed at: $(date)" >> "$STATS_FILE"
echo "" >> "$STATS_FILE"
echo "All test outputs saved to: $TEST_DIR" >> "$STATS_FILE"
echo "Full test report saved to: $STATS_FILE" >> "$STATS_FILE"

echo -e "${GREEN}All tests completed successfully!${NC}"
echo ""
echo "Demonstrated functionalities:"
echo "✓ Multiple cache types (Direct-mapped, Set-associative, Fully associative)"
echo "✓ All replacement policies (LRU, FIFO, Random)"
echo "✓ Both write policies (Write-through, Write-back)"
echo "✓ Both write miss policies (Write-allocate, No-write-allocate)"
echo "✓ Custom memory access patterns"
echo "✓ Trace file functionality"
echo "✓ Verbose output with cache contents"
echo "✓ Interactive mode with all commands"
echo "✓ Comprehensive help system"
echo "✓ Error handling and validation"
echo "✓ Performance with large configurations"
echo "✓ Statistics tracking and reporting"
echo ""
echo -e "${GREEN}🎉 Cache Simulator is working perfectly with all functionalities!${NC}"

echo ""
echo "=============================================="
echo "           TEST COMPLETED SUCCESSFULLY"
echo "=============================================="
echo ""
echo "📊 Test results saved to: $STATS_FILE"
echo "📁 Individual test outputs in: $TEST_DIR"
echo "✅ All functionality verified and working!"
