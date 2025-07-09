#include "set_associative_cache.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

SetAssociativeCache::SetAssociativeCache(
    size_t cache_size,
    size_t block_size,
    size_t associativity,
    std::unique_ptr<ReplacementPolicy> replacement_policy,
    WritePolicy write_policy,
    WriteMissPolicy write_miss_policy)
    : Cache(cache_size, block_size, associativity),
      replacement_policy_(std::move(replacement_policy)),
      write_policy_(write_policy),
      write_miss_policy_(write_miss_policy) {
    
    // Initialize cache structure
    cache_.resize(num_sets_);
    for (size_t i = 0; i < num_sets_; ++i) {
        cache_[i].resize(associativity_, CacheBlock(block_size_));
    }
}

Cache::AccessResult SetAssociativeCache::access(uint64_t address, Operation operation) {
    size_t set_index = getSetIndex(address);
    uint64_t tag = getTag(address);
    
    // Record access type
    if (operation == Operation::READ) {
        statistics_.recordRead();
    } else {
        statistics_.recordWrite();
    }
    
    // Check if block exists in cache
    int block_index = findBlock(set_index, tag);
    
    if (block_index != -1) {
        // Cache hit
        return handleHit(set_index, block_index, operation);
    } else {
        // Cache miss
        return handleMiss(address, set_index, tag, operation);
    }
}

int SetAssociativeCache::findBlock(size_t set_index, uint64_t tag) const {
    for (size_t i = 0; i < associativity_; ++i) {
        if (cache_[set_index][i].valid && cache_[set_index][i].tag == tag) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int SetAssociativeCache::findEmptyBlock(size_t set_index) const {
    for (size_t i = 0; i < associativity_; ++i) {
        if (!cache_[set_index][i].valid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

Cache::AccessResult SetAssociativeCache::handleHit(size_t set_index, size_t block_index, Operation operation) {
    // Update replacement policy
    replacement_policy_->updateOnAccess(set_index, block_index, true);
    
    if (operation == Operation::READ) {
        statistics_.recordHit();
        return AccessResult::HIT;
    } else {
        // Write hit
        statistics_.recordWriteHit();
        
        if (write_policy_ == WritePolicy::WRITE_BACK) {
            cache_[set_index][block_index].dirty = true;
        } else {
            // Write through - write to memory immediately
            writeToMemory(getTag(cache_[set_index][block_index].tag) << (getNumSets() > 1 ? static_cast<size_t>(std::log2(getNumSets())) : 0) | set_index);
        }
        
        return AccessResult::WRITE_HIT;
    }
}

Cache::AccessResult SetAssociativeCache::handleMiss(uint64_t address, size_t set_index, uint64_t tag, Operation operation) {
    if (operation == Operation::READ) {
        statistics_.recordMiss();
        // Always allocate on read miss
        allocateBlock(set_index, tag, operation);
        return AccessResult::MISS;
    } else {
        // Write miss
        statistics_.recordWriteMiss();
        
        if (write_miss_policy_ == WriteMissPolicy::WRITE_ALLOCATE) {
            // Allocate block and write to cache
            allocateBlock(set_index, tag, operation);
        } else {
            // No write allocate - write directly to memory
            writeToMemory(address);
        }
        
        return AccessResult::WRITE_MISS;
    }
}

size_t SetAssociativeCache::allocateBlock(size_t set_index, uint64_t tag, Operation operation) {
    // First, try to find an empty block
    int empty_block = findEmptyBlock(set_index);
    size_t victim_index;
    
    if (empty_block != -1) {
        victim_index = static_cast<size_t>(empty_block);
    } else {
        // Need to evict a block
        std::vector<bool> valid_blocks(associativity_);
        for (size_t i = 0; i < associativity_; ++i) {
            valid_blocks[i] = cache_[set_index][i].valid;
        }
        
        victim_index = replacement_policy_->selectVictim(set_index, valid_blocks);
        
        // If victim block is dirty (write-back policy), write it to memory
        if (cache_[set_index][victim_index].dirty) {
            size_t offset_bits = static_cast<size_t>(std::log2(block_size_));
            size_t index_bits = static_cast<size_t>(std::log2(num_sets_));
            uint64_t victim_address = (cache_[set_index][victim_index].tag << (offset_bits + index_bits)) | 
                                    (set_index << offset_bits);
            writeToMemory(victim_address);
        }
    }
    
    // Load new block
    cache_[set_index][victim_index].valid = true;
    cache_[set_index][victim_index].tag = tag;
    cache_[set_index][victim_index].dirty = (operation == Operation::WRITE && write_policy_ == WritePolicy::WRITE_BACK);
    
    // Read data from memory (simulated)
    size_t offset_bits = static_cast<size_t>(std::log2(block_size_));
    size_t index_bits = static_cast<size_t>(std::log2(num_sets_));
    readFromMemory((tag << (offset_bits + index_bits)) | (set_index << offset_bits));
    
    // Update replacement policy
    replacement_policy_->updateOnAccess(set_index, victim_index, false);
    
    return victim_index;
}

void SetAssociativeCache::writeToMemory(uint64_t address) {
    // Simulated memory write - in a real implementation, this would access main memory
    // For simulation purposes, we just track that a memory write occurred
}

void SetAssociativeCache::readFromMemory(uint64_t address) {
    // Simulated memory read - in a real implementation, this would access main memory
    // For simulation purposes, we just track that a memory read occurred
}

CacheStatistics SetAssociativeCache::getStatistics() const {
    return statistics_;
}

void SetAssociativeCache::resetStatistics() {
    statistics_.reset();
}

void SetAssociativeCache::clear() {
    for (auto& set : cache_) {
        for (auto& block : set) {
            block.valid = false;
            block.dirty = false;
            block.tag = 0;
            std::fill(block.data.begin(), block.data.end(), 0);
        }
    }
    replacement_policy_->reset();
    statistics_.reset();
}

std::string SetAssociativeCache::getConfig() const {
    std::ostringstream oss;
    oss << Cache::getConfig();
    oss << "  Replacement Policy: " << replacement_policy_->getName() << "\n";
    oss << "  Write Policy: " << (write_policy_ == WritePolicy::WRITE_THROUGH ? "Write-Through" : "Write-Back") << "\n";
    oss << "  Write Miss Policy: " << (write_miss_policy_ == WriteMissPolicy::WRITE_ALLOCATE ? "Write-Allocate" : "No-Write-Allocate") << "\n";
    
    return oss.str();
}

void SetAssociativeCache::printCacheContents() const {
    std::cout << "\nCache Contents:\n";
    std::cout << "================\n";
    
    for (size_t set = 0; set < num_sets_; ++set) {
        std::cout << "Set " << set << ": ";
        for (size_t way = 0; way < associativity_; ++way) {
            const auto& block = cache_[set][way];
            if (block.valid) {
                std::cout << "[V:" << (block.valid ? "1" : "0") 
                         << " D:" << (block.dirty ? "1" : "0")
                         << " Tag:0x" << std::hex << block.tag << std::dec << "] ";
            } else {
                std::cout << "[Invalid] ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "================\n\n";
}
