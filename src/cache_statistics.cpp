#include "cache_statistics.h"
#include <sstream>
#include <iomanip>

CacheStatistics::CacheStatistics()
    : hits_(0), misses_(0), reads_(0), writes_(0), write_hits_(0), write_misses_(0) {
}

void CacheStatistics::reset() {
    hits_ = 0;
    misses_ = 0;
    reads_ = 0;
    writes_ = 0;
    write_hits_ = 0;
    write_misses_ = 0;
}

void CacheStatistics::recordHit() {
    hits_++;
}

void CacheStatistics::recordMiss() {
    misses_++;
}

void CacheStatistics::recordWriteHit() {
    write_hits_++;
    hits_++;
}

void CacheStatistics::recordWriteMiss() {
    write_misses_++;
    misses_++;
}

void CacheStatistics::recordRead() {
    reads_++;
}

void CacheStatistics::recordWrite() {
    writes_++;
}

double CacheStatistics::getHitRate() const {
    uint64_t total = hits_ + misses_;
    return total > 0 ? (static_cast<double>(hits_) / total) * 100.0 : 0.0;
}

double CacheStatistics::getMissRate() const {
    uint64_t total = hits_ + misses_;
    return total > 0 ? (static_cast<double>(misses_) / total) * 100.0 : 0.0;
}

double CacheStatistics::getReadHitRate() const {
    uint64_t read_hits = hits_ - write_hits_;
    uint64_t read_misses = misses_ - write_misses_;
    uint64_t total_reads = read_hits + read_misses;
    return total_reads > 0 ? (static_cast<double>(read_hits) / total_reads) * 100.0 : 0.0;
}

double CacheStatistics::getWriteHitRate() const {
    uint64_t total_writes = write_hits_ + write_misses_;
    return total_writes > 0 ? (static_cast<double>(write_hits_) / total_writes) * 100.0 : 0.0;
}

std::string CacheStatistics::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    oss << "Cache Statistics:\n";
    oss << "  Total Accesses: " << getTotalAccesses() << "\n";
    oss << "  Hits: " << hits_ << "\n";
    oss << "  Misses: " << misses_ << "\n";
    oss << "  Hit Rate: " << getHitRate() << "%\n";
    oss << "  Miss Rate: " << getMissRate() << "%\n";
    oss << "\n";
    oss << "  Read Accesses: " << reads_ << "\n";
    oss << "  Write Accesses: " << writes_ << "\n";
    oss << "  Read Hit Rate: " << getReadHitRate() << "%\n";
    oss << "  Write Hit Rate: " << getWriteHitRate() << "%\n";
    oss << "\n";
    oss << "  Read Hits: " << (hits_ - write_hits_) << "\n";
    oss << "  Read Misses: " << (misses_ - write_misses_) << "\n";
    oss << "  Write Hits: " << write_hits_ << "\n";
    oss << "  Write Misses: " << write_misses_ << "\n";
    
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const CacheStatistics& stats) {
    os << stats.toString();
    return os;
}
