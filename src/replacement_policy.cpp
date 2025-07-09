#include "replacement_policy.h"
#include <iostream>
#include <random>
#include <string>
#include <algorithm>
#include <stdexcept>

LRUPolicy::LRUPolicy(size_t num_sets, size_t associativity)
    : num_sets_(num_sets), associativity_(associativity), access_order_(num_sets, std::vector<uint64_t>(associativity, 0)), global_time_(0) {
}

size_t LRUPolicy::selectVictim(size_t set_index, const std::vector<bool>& valid_blocks) {
    size_t oldest_block_index = 0;
    uint64_t oldest_time = access_order_[set_index][0];
    for (size_t i = 1; i < associativity_; ++i) {
        if (valid_blocks[i] && access_order_[set_index][i] < oldest_time) {
            oldest_time = access_order_[set_index][i];
            oldest_block_index = i;
        }
    }
    return oldest_block_index;
}

void LRUPolicy::updateOnAccess(size_t set_index, size_t block_index, bool hit) {
    access_order_[set_index][block_index] = ++global_time_;
}

void LRUPolicy::reset() {
    global_time_ = 0;
    for (auto& set_order : access_order_) {
        std::fill(set_order.begin(), set_order.end(), 0);
    }
}

FIFOPolicy::FIFOPolicy(size_t num_sets, size_t associativity)
    : num_sets_(num_sets), associativity_(associativity), insertion_order_(num_sets, std::vector<uint64_t>(associativity, 0)), global_time_(0) {
}

size_t FIFOPolicy::selectVictim(size_t set_index, const std::vector<bool>& valid_blocks) {
    size_t first_in_block_index = 0;
    uint64_t first_in_time = insertion_order_[set_index][0];
    for (size_t i = 1; i < associativity_; ++i) {
        if (valid_blocks[i] && insertion_order_[set_index][i] < first_in_time) {
            first_in_time = insertion_order_[set_index][i];
            first_in_block_index = i;
        }
    }
    return first_in_block_index;
}

void FIFOPolicy::updateOnAccess(size_t set_index, size_t block_index, bool hit) {
    if (!hit) {
        insertion_order_[set_index][block_index] = ++global_time_;
    }
}

void FIFOPolicy::reset() {
    global_time_ = 0;
    for (auto& set_order : insertion_order_) {
        std::fill(set_order.begin(), set_order.end(), 0);
    }
}

RandomPolicy::RandomPolicy(size_t num_sets, size_t associativity)
    : num_sets_(num_sets), associativity_(associativity) {
}

size_t RandomPolicy::selectVictim(size_t set_index, const std::vector<bool>& valid_blocks) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, associativity_ - 1);

    size_t victim;
    do {
        victim = dis(gen);
    } while (!valid_blocks[victim]);

    return victim;
}

void RandomPolicy::updateOnAccess(size_t set_index, size_t block_index, bool hit) {
    // No state to update for random policy
}

void RandomPolicy::reset() {
    // No state to reset for random policy
}

// Factory implementations
std::unique_ptr<ReplacementPolicy> ReplacementPolicyFactory::createPolicy(
    PolicyType type, size_t num_sets, size_t associativity) {
    
    switch (type) {
        case PolicyType::LRU:
            return std::make_unique<LRUPolicy>(num_sets, associativity);
        case PolicyType::FIFO:
            return std::make_unique<FIFOPolicy>(num_sets, associativity);
        case PolicyType::RANDOM:
            return std::make_unique<RandomPolicy>(num_sets, associativity);
        default:
            throw std::invalid_argument("Unknown replacement policy type");
    }
}

ReplacementPolicyFactory::PolicyType ReplacementPolicyFactory::stringToPolicy(const std::string& policy_str) {
    if (policy_str == "LRU" || policy_str == "lru") {
        return PolicyType::LRU;
    } else if (policy_str == "FIFO" || policy_str == "fifo") {
        return PolicyType::FIFO;
    } else if (policy_str == "RANDOM" || policy_str == "random") {
        return PolicyType::RANDOM;
    } else {
        throw std::invalid_argument("Unknown replacement policy: " + policy_str);
    }
}
