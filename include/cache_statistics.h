#ifndef CACHE_STATISTICS_H
#define CACHE_STATISTICS_H

#include <cstdint>
#include <string>
#include <ostream>

/**
 * @brief Cache statistics class
 */
class CacheStatistics {
public:
    /**
     * @brief Constructor
     */
    CacheStatistics();

    /**
     * @brief Reset all statistics
     */
    void reset();

    /**
     * @brief Record a cache hit
     */
    void recordHit();

    /**
     * @brief Record a cache miss
     */
    void recordMiss();

    /**
     * @brief Record a write hit
     */
    void recordWriteHit();

    /**
     * @brief Record a write miss
     */
    void recordWriteMiss();

    /**
     * @brief Record a read access
     */
    void recordRead();

    /**
     * @brief Record a write access
     */
    void recordWrite();

    // Getters
    uint64_t getHits() const { return hits_; }
    uint64_t getMisses() const { return misses_; }
    uint64_t getReads() const { return reads_; }
    uint64_t getWrites() const { return writes_; }
    uint64_t getWriteHits() const { return write_hits_; }
    uint64_t getWriteMisses() const { return write_misses_; }
    uint64_t getTotalAccesses() const { return hits_ + misses_; }

    /**
     * @brief Calculate hit rate
     * @return Hit rate as percentage
     */
    double getHitRate() const;

    /**
     * @brief Calculate miss rate
     * @return Miss rate as percentage
     */
    double getMissRate() const;

    /**
     * @brief Calculate read hit rate
     * @return Read hit rate as percentage
     */
    double getReadHitRate() const;

    /**
     * @brief Calculate write hit rate
     * @return Write hit rate as percentage
     */
    double getWriteHitRate() const;

    /**
     * @brief Get statistics as formatted string
     * @return Statistics string
     */
    std::string toString() const;

    /**
     * @brief Output operator
     */
    friend std::ostream& operator<<(std::ostream& os, const CacheStatistics& stats);

private:
    uint64_t hits_;         // Total cache hits
    uint64_t misses_;       // Total cache misses
    uint64_t reads_;        // Total read accesses
    uint64_t writes_;       // Total write accesses
    uint64_t write_hits_;   // Write hits
    uint64_t write_misses_; // Write misses
};

#endif // CACHE_STATISTICS_H
