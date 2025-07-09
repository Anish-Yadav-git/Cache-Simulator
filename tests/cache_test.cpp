#include "set_associative_cache.h"
#include "replacement_policy.h"
#include <iostream>
#include <vector>
#include <cassert>

void testDirectMappedCache() {
    std::cout << "\n=== Testing Direct-Mapped Cache ===\n";
    
    size_t cache_size = 512;   // 512B cache
    size_t block_size = 32;    // 32B blocks
    size_t associativity = 1;  // Direct-mapped
    
    auto policy = ReplacementPolicyFactory::createPolicy(
        ReplacementPolicyFactory::PolicyType::LRU,
        cache_size / (block_size * associativity),
        associativity
    );
    
    SetAssociativeCache cache(cache_size, block_size, associativity, std::move(policy));
    
    std::cout << cache.getConfig() << std::endl;
    
    // Test access pattern that demonstrates direct-mapped behavior
    std::vector<uint64_t> addresses = {0x0, 0x200, 0x0, 0x200};
    
    for (auto addr : addresses) {
        auto result = cache.access(addr, Cache::Operation::READ);
        std::cout << "Access 0x" << std::hex << addr << " -> " 
                  << (result == Cache::AccessResult::HIT ? "HIT" : "MISS") << std::endl;
    }
    
    std::cout << cache.getStatistics() << std::endl;
}

void testFullyAssociativeCache() {
    std::cout << "\n=== Testing Fully Associative Cache ===\n";
    
    size_t cache_size = 512;   // 512B cache
    size_t block_size = 32;    // 32B blocks
    size_t associativity = 0;  // Fully associative
    
    auto policy = ReplacementPolicyFactory::createPolicy(
        ReplacementPolicyFactory::PolicyType::LRU,
        1, // Only 1 set for fully associative
        cache_size / block_size // All blocks in one set
    );
    
    SetAssociativeCache cache(cache_size, block_size, associativity, std::move(policy));
    
    std::cout << cache.getConfig() << std::endl;
    
    // Access multiple addresses that would conflict in direct-mapped
    std::vector<uint64_t> addresses = {0x0, 0x200, 0x400, 0x600, 0x0, 0x200};
    
    for (auto addr : addresses) {
        auto result = cache.access(addr, Cache::Operation::READ);
        std::cout << "Access 0x" << std::hex << addr << " -> " 
                  << (result == Cache::AccessResult::HIT ? "HIT" : "MISS") << std::endl;
    }
    
    std::cout << cache.getStatistics() << std::endl;
}

void testSetAssociativeCache() {
    std::cout << "\n=== Testing 2-way Set Associative Cache ===\n";
    
    size_t cache_size = 512;   // 512B cache
    size_t block_size = 32;    // 32B blocks
    size_t associativity = 2;  // 2-way set associative
    
    auto policy = ReplacementPolicyFactory::createPolicy(
        ReplacementPolicyFactory::PolicyType::LRU,
        cache_size / (block_size * associativity),
        associativity
    );
    
    SetAssociativeCache cache(cache_size, block_size, associativity, std::move(policy));
    
    std::cout << cache.getConfig() << std::endl;
    
    // Test access pattern
    std::vector<uint64_t> addresses = {0x0, 0x100, 0x200, 0x0, 0x100};
    
    for (auto addr : addresses) {
        auto result = cache.access(addr, Cache::Operation::READ);
        std::cout << "Access 0x" << std::hex << addr << " -> " 
                  << (result == Cache::AccessResult::HIT ? "HIT" : "MISS") << std::endl;
    }
    
    cache.printCacheContents();
    std::cout << cache.getStatistics() << std::endl;
}

void testReplacementPolicies() {
    std::cout << "\n=== Testing Different Replacement Policies ===\n";
    
    size_t cache_size = 128;   // Small cache for easy testing
    size_t block_size = 16;    
    size_t associativity = 2;  
    
    std::vector<ReplacementPolicyFactory::PolicyType> policies = {
        ReplacementPolicyFactory::PolicyType::LRU,
        ReplacementPolicyFactory::PolicyType::FIFO,
        ReplacementPolicyFactory::PolicyType::RANDOM
    };
    
    std::vector<std::string> policy_names = {"LRU", "FIFO", "RANDOM"};
    
    // Access pattern that will cause evictions
    std::vector<uint64_t> addresses = {0x0, 0x10, 0x80, 0x0, 0x10, 0x90};
    
    for (size_t i = 0; i < policies.size(); ++i) {
        std::cout << "\n--- " << policy_names[i] << " Policy ---\n";
        
        auto policy = ReplacementPolicyFactory::createPolicy(
            policies[i],
            cache_size / (block_size * associativity),
            associativity
        );
        
        SetAssociativeCache cache(cache_size, block_size, associativity, std::move(policy));
        
        for (auto addr : addresses) {
            auto result = cache.access(addr, Cache::Operation::READ);
            std::cout << "Access 0x" << std::hex << addr << " -> " 
                      << (result == Cache::AccessResult::HIT ? "HIT" : "MISS") << std::endl;
        }
        
        std::cout << "Hit Rate: " << cache.getStatistics().getHitRate() << "%\n";
    }
}

void testWritePolicies() {
    std::cout << "\n=== Testing Write Policies ===\n";
    
    size_t cache_size = 256;
    size_t block_size = 32;
    size_t associativity = 2;
    
    // Test Write-Through
    std::cout << "\n--- Write-Through Policy ---\n";
    {
        auto policy = ReplacementPolicyFactory::createPolicy(
            ReplacementPolicyFactory::PolicyType::LRU,
            cache_size / (block_size * associativity),
            associativity
        );
        
        SetAssociativeCache cache(cache_size, block_size, associativity, std::move(policy),
                                 SetAssociativeCache::WritePolicy::WRITE_THROUGH);
        
        cache.access(0x0, Cache::Operation::READ);   // Miss, load block
        cache.access(0x0, Cache::Operation::WRITE);  // Hit, write through
        cache.access(0x0, Cache::Operation::READ);   // Hit
        
        std::cout << cache.getStatistics() << std::endl;
    }
    
    // Test Write-Back
    std::cout << "\n--- Write-Back Policy ---\n";
    {
        auto policy = ReplacementPolicyFactory::createPolicy(
            ReplacementPolicyFactory::PolicyType::LRU,
            cache_size / (block_size * associativity),
            associativity
        );
        
        SetAssociativeCache cache(cache_size, block_size, associativity, std::move(policy),
                                 SetAssociativeCache::WritePolicy::WRITE_BACK);
        
        cache.access(0x0, Cache::Operation::READ);   // Miss, load block
        cache.access(0x0, Cache::Operation::WRITE);  // Hit, mark dirty
        cache.access(0x0, Cache::Operation::READ);   // Hit
        
        std::cout << cache.getStatistics() << std::endl;
        cache.printCacheContents();
    }
}

int main() {
    std::cout << "Cache Simulator Test Suite\n";
    std::cout << "==========================\n";
    
    testDirectMappedCache();
    testFullyAssociativeCache();
    testSetAssociativeCache();
    testReplacementPolicies();
    testWritePolicies();
    
    std::cout << "\nAll tests completed!\n";
    return 0;
}
