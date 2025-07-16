#ifndef SET_ASSOCIATIVE_CACHE_H
#define SET_ASSOCIATIVE_CACHE_H

#include "cache.h"
#include "cache_statistics.h"
#include "replacement_policy.h"
#include <vector>
#include <memory>

/**
 * @brief Cache block/line structure
 */
struct CacheBlock {
    bool valid;          // Valid bit
    bool dirty;          // Dirty bit (for write-back policy)
    uint64_t tag;        // Tag bits
    std::vector<uint8_t> data; // Block data (optional for simulation)

    CacheBlock(size_t block_size = 0) 
        : valid(false), dirty(false), tag(0), data(block_size, 0) {}
};

/**
 * @brief Set-associative cache implementation
 */
class SetAssociativeCache : public Cache {
public:
    /**
     * @brief Write policy
     */
    enum class WritePolicy {
        WRITE_THROUGH,
        WRITE_BACK
    };

    /**
     * @brief Write miss policy
     */
    enum class WriteMissPolicy {
        WRITE_ALLOCATE,
        NO_WRITE_ALLOCATE
    };

    /**
     * @brief Constructor
     * @param cache_size Total cache size in bytes
     * @param block_size Block size in bytes
     * @param associativity Associativity (1 for direct-mapped, 0 for fully associative)
     * @param replacement_policy Replacement policy
     * @param write_policy Write policy
     * @param write_miss_policy Write miss policy
     */
    SetAssociativeCache(
        size_t cache_size,
        size_t block_size,
        size_t associativity,
        std::unique_ptr<ReplacementPolicy> replacement_policy,
        WritePolicy write_policy = WritePolicy::WRITE_THROUGH,
        WriteMissPolicy write_miss_policy = WriteMissPolicy::WRITE_ALLOCATE
    );

    /**
     * @brief Destructor
     */
    ~SetAssociativeCache() override = default;

    /**
     * @brief Access cache
     * @param address Memory address
     * @param operation Operation type
     * @return Access result
     */
    AccessResult access(uint64_t address, Operation operation) override;

    /**
     * @brief Get cache statistics
     * @return Statistics object
     */
    CacheStatistics getStatistics() const override;

    /**
     * @brief Reset cache statistics
     */
    void resetStatistics() override;

    /**
     * @brief Clear cache contents
     */
    void clear() override;

    /**
     * @brief Get cache configuration
     * @return Configuration string
     */
    std::string getConfig() const override;

    /**
     * @brief Print cache contents (for debugging)
     */
    void printCacheContents() const;

    /**
     * @brief Get write policy
     * @return Write policy
     */
    WritePolicy getWritePolicy() const { return write_policy_; }

    /**
     * @brief Get write miss policy
     * @return Write miss policy
     */
    WriteMissPolicy getWriteMissPolicy() const { return write_miss_policy_; }

    /**
     * @brief Get cache contents for visualization
     * @return Vector of cache sets with their blocks
     */
    const std::vector<std::vector<CacheBlock>>& getCacheContents() const { return cache_; }

    /**
     * @brief Get cache contents as JSON string for GUI
     * @return JSON string representation of cache contents
     */
    std::string getCacheContentsJSON() const;

    /**
     * @brief Check if a block is valid
     * @param set_index Set index
     * @param block_index Block index
     * @return True if block is valid, false otherwise
     */
    bool isBlockValid(size_t set_index, size_t block_index) const;

    /**
     * @brief Check if a block is dirty
     * @param set_index Set index
     * @param block_index Block index
     * @return True if block is dirty, false otherwise
     */
    bool isBlockDirty(size_t set_index, size_t block_index) const;

    /**
     * @brief Get block tag
     * @param set_index Set index
     * @param block_index Block index
     * @return Block tag
     */
    uint64_t getBlockTag(size_t set_index, size_t block_index) const;

private:
    std::vector<std::vector<CacheBlock>> cache_; // Cache storage
    std::unique_ptr<ReplacementPolicy> replacement_policy_;
    CacheStatistics statistics_;
    WritePolicy write_policy_;
    WriteMissPolicy write_miss_policy_;

    /**
     * @brief Find block in set
     * @param set_index Set index
     * @param tag Tag to search for
     * @return Block index if found, -1 otherwise
     */
    int findBlock(size_t set_index, uint64_t tag) const;

    /**
     * @brief Find empty block in set
     * @param set_index Set index
     * @return Block index if found, -1 if set is full
     */
    int findEmptyBlock(size_t set_index) const;

    /**
     * @brief Handle cache hit
     * @param set_index Set index
     * @param block_index Block index
     * @param operation Operation type
     * @return Access result
     */
    AccessResult handleHit(size_t set_index, size_t block_index, Operation operation);

    /**
     * @brief Handle cache miss
     * @param address Memory address
     * @param set_index Set index
     * @param tag Tag
     * @param operation Operation type
     * @return Access result
     */
    AccessResult handleMiss(uint64_t address, size_t set_index, uint64_t tag, Operation operation);

    /**
     * @brief Allocate block in cache
     * @param set_index Set index
     * @param tag Tag
     * @param operation Operation type
     * @return Block index where data was allocated
     */
    size_t allocateBlock(size_t set_index, uint64_t tag, Operation operation);

    /**
     * @brief Write data to memory (simulated)
     * @param address Memory address
     */
    void writeToMemory(uint64_t address);

    /**
     * @brief Read data from memory (simulated)
     * @param address Memory address
     */
    void readFromMemory(uint64_t address);
};

#endif // SET_ASSOCIATIVE_CACHE_H
