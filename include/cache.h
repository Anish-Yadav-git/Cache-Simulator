#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ReplacementPolicy;
class CacheStatistics;

/**
 * @brief Abstract base class for cache implementations
 */
class Cache {
public:
    /**
     * @brief Cache access result
     */
    enum class AccessResult {
        HIT,
        MISS,
        WRITE_HIT,
        WRITE_MISS
    };

    /**
     * @brief Cache operation type
     */
    enum class Operation {
        READ,
        WRITE
    };

    /**
     * @brief Constructor
     * @param cache_size Total cache size in bytes
     * @param block_size Block size in bytes
     * @param associativity Associativity (1 for direct-mapped, 0 for fully associative)
     */
    Cache(size_t cache_size, size_t block_size, size_t associativity);

    /**
     * @brief Virtual destructor
     */
    virtual ~Cache() = default;

    /**
     * @brief Access cache (read or write)
     * @param address Memory address
     * @param operation Operation type (READ/WRITE)
     * @return Access result
     */
    virtual AccessResult access(uint64_t address, Operation operation) = 0;

    /**
     * @brief Get cache statistics
     * @return Statistics object
     */
    virtual CacheStatistics getStatistics() const = 0;

    /**
     * @brief Reset cache statistics
     */
    virtual void resetStatistics() = 0;

    /**
     * @brief Clear cache contents
     */
    virtual void clear() = 0;

    /**
     * @brief Get cache configuration as string
     * @return Configuration string
     */
    virtual std::string getConfig() const;

    // Getters
    size_t getCacheSize() const { return cache_size_; }
    size_t getBlockSize() const { return block_size_; }
    size_t getAssociativity() const { return associativity_; }
    size_t getNumSets() const { return num_sets_; }
    size_t getNumBlocks() const { return num_blocks_; }

protected:
    size_t cache_size_;      // Total cache size in bytes
    size_t block_size_;      // Block size in bytes
    size_t associativity_;   // Associativity
    size_t num_sets_;        // Number of sets
    size_t num_blocks_;      // Total number of blocks

    /**
     * @brief Extract set index from address
     * @param address Memory address
     * @return Set index
     */
    size_t getSetIndex(uint64_t address) const;

    /**
     * @brief Extract tag from address
     * @param address Memory address
     * @return Tag
     */
    uint64_t getTag(uint64_t address) const;

    /**
     * @brief Extract block offset from address
     * @param address Memory address
     * @return Block offset
     */
    size_t getBlockOffset(uint64_t address) const;

protected:
    size_t offset_bits_;     // Number of offset bits
    size_t index_bits_;      // Number of index bits
    uint64_t tag_mask_;      // Tag mask
    uint64_t index_mask_;    // Index mask
    uint64_t offset_mask_;   // Offset mask

private:
    void calculateBitMasks();
};

#endif // CACHE_H
