#include "cache.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

Cache::Cache(size_t cache_size, size_t block_size, size_t associativity)
    : cache_size_(cache_size), block_size_(block_size), associativity_(associativity) {
    
    // Validate parameters
    if (cache_size == 0 || block_size == 0) {
        throw std::invalid_argument("Cache size and block size must be greater than 0");
    }
    
    if (cache_size % block_size != 0) {
        throw std::invalid_argument("Cache size must be a multiple of block size");
    }
    
    // Calculate derived values
    num_blocks_ = cache_size_ / block_size_;
    
    if (associativity_ == 0) {
        // Fully associative
        num_sets_ = 1;
        associativity_ = num_blocks_;
    } else {
        // Set associative or direct mapped
        if (num_blocks_ % associativity_ != 0) {
            throw std::invalid_argument("Number of blocks must be divisible by associativity");
        }
        num_sets_ = num_blocks_ / associativity_;
    }
    
    calculateBitMasks();
}

void Cache::calculateBitMasks() {
    // Calculate number of bits for each field
    offset_bits_ = static_cast<size_t>(std::log2(block_size_));
    index_bits_ = static_cast<size_t>(std::log2(num_sets_));
    
    // Create masks
    offset_mask_ = (1ULL << offset_bits_) - 1;
    index_mask_ = (1ULL << index_bits_) - 1;
    tag_mask_ = ~((1ULL << (offset_bits_ + index_bits_)) - 1);
}

size_t Cache::getSetIndex(uint64_t address) const {
    return (address >> offset_bits_) & index_mask_;
}

uint64_t Cache::getTag(uint64_t address) const {
    return address >> (offset_bits_ + index_bits_);
}

size_t Cache::getBlockOffset(uint64_t address) const {
    return address & offset_mask_;
}

std::string Cache::getConfig() const {
    std::ostringstream oss;
    oss << "Cache Configuration:\n";
    oss << "  Cache Size: " << cache_size_ << " bytes\n";
    oss << "  Block Size: " << block_size_ << " bytes\n";
    oss << "  Associativity: ";
    if (associativity_ == num_blocks_) {
        oss << "Fully Associative\n";
    } else if (associativity_ == 1) {
        oss << "Direct Mapped\n";
    } else {
        oss << associativity_ << "-way\n";
    }
    oss << "  Number of Sets: " << num_sets_ << "\n";
    oss << "  Number of Blocks: " << num_blocks_ << "\n";
    oss << "  Offset Bits: " << offset_bits_ << "\n";
    oss << "  Index Bits: " << index_bits_ << "\n";
    oss << "  Tag Bits: " << (64 - offset_bits_ - index_bits_) << "\n";
    
    return oss.str();
}
