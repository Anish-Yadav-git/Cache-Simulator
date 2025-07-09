#ifndef REPLACEMENT_POLICY_H
#define REPLACEMENT_POLICY_H

#include <cstdint>
#include <vector>
#include <memory>

/**
 * @brief Abstract base class for replacement policies
 */
class ReplacementPolicy {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~ReplacementPolicy() = default;

    /**
     * @brief Select victim block for replacement
     * @param set_index Set index
     * @param valid_blocks Vector indicating which blocks are valid
     * @return Index of block to replace
     */
    virtual size_t selectVictim(size_t set_index, const std::vector<bool>& valid_blocks) = 0;

    /**
     * @brief Update policy state on cache access
     * @param set_index Set index
     * @param block_index Block index within the set
     * @param hit Whether this was a hit or miss
     */
    virtual void updateOnAccess(size_t set_index, size_t block_index, bool hit) = 0;

    /**
     * @brief Reset policy state
     */
    virtual void reset() = 0;

    /**
     * @brief Get policy name
     * @return Policy name as string
     */
    virtual std::string getName() const = 0;
};

/**
 * @brief Least Recently Used (LRU) replacement policy
 */
class LRUPolicy : public ReplacementPolicy {
public:
    /**
     * @brief Constructor
     * @param num_sets Number of sets in cache
     * @param associativity Associativity of cache
     */
    LRUPolicy(size_t num_sets, size_t associativity);

    size_t selectVictim(size_t set_index, const std::vector<bool>& valid_blocks) override;
    void updateOnAccess(size_t set_index, size_t block_index, bool hit) override;
    void reset() override;
    std::string getName() const override { return "LRU"; }

private:
    size_t num_sets_;
    size_t associativity_;
    std::vector<std::vector<uint64_t>> access_order_; // Access order for each set
    uint64_t global_time_;
};

/**
 * @brief First In First Out (FIFO) replacement policy
 */
class FIFOPolicy : public ReplacementPolicy {
public:
    /**
     * @brief Constructor
     * @param num_sets Number of sets in cache
     * @param associativity Associativity of cache
     */
    FIFOPolicy(size_t num_sets, size_t associativity);

    size_t selectVictim(size_t set_index, const std::vector<bool>& valid_blocks) override;
    void updateOnAccess(size_t set_index, size_t block_index, bool hit) override;
    void reset() override;
    std::string getName() const override { return "FIFO"; }

private:
    size_t num_sets_;
    size_t associativity_;
    std::vector<std::vector<uint64_t>> insertion_order_; // Insertion order for each set
    uint64_t global_time_;
};

/**
 * @brief Random replacement policy
 */
class RandomPolicy : public ReplacementPolicy {
public:
    /**
     * @brief Constructor
     * @param num_sets Number of sets in cache
     * @param associativity Associativity of cache
     */
    RandomPolicy(size_t num_sets, size_t associativity);

    size_t selectVictim(size_t set_index, const std::vector<bool>& valid_blocks) override;
    void updateOnAccess(size_t set_index, size_t block_index, bool hit) override;
    void reset() override;
    std::string getName() const override { return "Random"; }

private:
    size_t num_sets_;
    size_t associativity_;
};

/**
 * @brief Factory class for creating replacement policies
 */
class ReplacementPolicyFactory {
public:
    /**
     * @brief Policy types
     */
    enum class PolicyType {
        LRU,
        FIFO,
        RANDOM
    };

    /**
     * @brief Create replacement policy
     * @param type Policy type
     * @param num_sets Number of sets in cache
     * @param associativity Associativity of cache
     * @return Unique pointer to replacement policy
     */
    static std::unique_ptr<ReplacementPolicy> createPolicy(
        PolicyType type, size_t num_sets, size_t associativity);

    /**
     * @brief Convert string to policy type
     * @param policy_str Policy string
     * @return Policy type
     */
    static PolicyType stringToPolicy(const std::string& policy_str);
};

#endif // REPLACEMENT_POLICY_H
