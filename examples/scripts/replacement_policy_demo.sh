#!/bin/bash

# Build the project (assumes CMake and Makefile are correctly set up)
# Navigate to project root for building
PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$PROJECT_ROOT"
cmake -S . -B build
make -C build

# Fixed cache configuration
CACHE_SIZE=512      # bytes
BLOCK_SIZE=32       # bytes
ASSOCIATIVITY=2     # way set associative

# Sequence of memory addresses 
ADDRESSES="0x0 0x20 0x40 0x60 0x80 0xA0 0xC0 0xE0 0x100 0x120"

# Policies
POLICIES=("LRU" "FIFO" "RANDOM")

# Display result header
printf "\n=== Replacement Policy Demonstration ===\n"

# Create examples/output directory if it doesn't exist
directory="examples/output"
if [ ! -d "$directory" ]; then
  mkdir -p "$directory"
fi

# Loop over each policy
for POLICY in "${POLICIES[@]}"
do
    
    # Output file
    OUTPUT_FILE="$directory/replacement_policy_stats_$POLICY.txt"

    # Run simulator with verbose mode
    ./bin/cache_simulator -v -s $CACHE_SIZE -b $BLOCK_SIZE -a $ASSOCIATIVITY -r $POLICY \
    -A "$ADDRESSES" -o $OUTPUT_FILE

    # Display a short summary for each policy
    printf "\nPolicy: $POLICY\n"
    cat $OUTPUT_FILE | grep -E 'Eviction|Final'  # Short summary just for demonstration purposes

done

printf "\n=== Demonstration Completed ===\n"
