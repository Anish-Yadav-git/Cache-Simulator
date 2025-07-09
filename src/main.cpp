#include "set_associative_cache.h"
#include "replacement_policy.h"
#include "cache_statistics.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <getopt.h>
#include <fstream>
#include <iomanip>
#include <chrono>

// Configuration structure
struct CacheConfig {
    size_t cache_size = 1024;           // Default: 1KB
    size_t block_size = 32;             // Default: 32B
    size_t associativity = 4;           // Default: 4-way
    std::string replacement_policy = "LRU"; // Default: LRU
    std::string write_policy = "WRITE_THROUGH"; // Default: Write-through
    std::string write_miss_policy = "WRITE_ALLOCATE"; // Default: Write-allocate
    std::vector<uint64_t> addresses;    // Addresses to access
    std::vector<std::string> operations; // Operations (READ/WRITE)
    std::string trace_file = "";        // Input trace file
    std::string output_file = "stats.txt"; // Output statistics file
    bool interactive = false;           // Interactive mode
    bool help = false;                  // Show help
    bool verbose = false;               // Verbose output
    bool quiet = false;                 // Suppress console output
};

void printHelp(const char* program_name) {
    std::cout << "Cache Simulator - Command Line Interface\n";
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -s, --cache-size SIZE      Cache size in bytes (default: 1024)\n";
    std::cout << "  -b, --block-size SIZE      Block size in bytes (default: 32)\n";
    std::cout << "  -a, --associativity N      Associativity (1=direct, 0=fully, default: 4)\n";
    std::cout << "  -r, --replacement POLICY   Replacement policy: LRU|FIFO|RANDOM (default: LRU)\n";
    std::cout << "  -w, --write-policy POLICY  Write policy: WRITE_THROUGH|WRITE_BACK (default: WRITE_THROUGH)\n";
    std::cout << "  -m, --write-miss POLICY    Write miss: WRITE_ALLOCATE|NO_WRITE_ALLOCATE (default: WRITE_ALLOCATE)\n";
    std::cout << "  -t, --trace-file FILE      Input trace file with memory accesses\n";
    std::cout << "  -o, --output-file FILE     Output statistics file (default: stats.txt)\n";
    std::cout << "  -A, --addresses ADDRS      Comma-separated hex addresses (e.g., 0x0,0x20,0x40)\n";
    std::cout << "  -O, --operations OPS       Comma-separated operations (e.g., read,WRITE,read)\n";
    std::cout << "  -i, --interactive          Interactive mode\n";
    std::cout << "  -v, --verbose              Verbose output\n";
    std::cout << "  -q, --quiet                Suppress console output (useful with trace files)\n";
    std::cout << "  -h, --help                 Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " --cache-size 2048 --associativity 8 --replacement LRU\n";
    std::cout << "  " << program_name << " -s 512 -b 16 -a 2 -r FIFO --addresses 0x0,0x10,0x20\n";
    std::cout << "  " << program_name << " --interactive\n";
    std::cout << "  " << program_name << " -t trace.txt -o results.txt -q\n";
    std::cout << "  " << program_name << " --trace-file memory_trace.txt --verbose\n\n";
    std::cout << "Trace File Format:\n";
    std::cout << "  Each line: <operation> <address>\n";
    std::cout << "  Example: R 0x400000\n";
    std::cout << "           W 0x400004\n";
    std::cout << "           READ 0x400008\n";
    std::cout << "           WRITE 0x40000C\n\n";
    std::cout << "Note: If no trace file or addresses are specified, default test pattern will be used.\n";
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

uint64_t parseAddress(const std::string& addr_str) {
    try {
        return std::stoull(addr_str, nullptr, 0); // Auto-detect base (0x for hex)
    } catch (const std::exception& e) {
        std::cerr << "Error parsing address '" << addr_str << "': " << e.what() << std::endl;
        return 0;
    }
}

// Function to read trace file and populate addresses and operations
bool readTraceFile(const std::string& filename, std::vector<uint64_t>& addresses, std::vector<std::string>& operations) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open trace file '" << filename << "'" << std::endl;
        return false;
    }
    
    std::string line;
    size_t line_number = 0;
    addresses.clear();
    operations.clear();
    
    while (std::getline(file, line)) {
        line_number++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string operation, address_str;
        
        if (!(iss >> operation >> address_str)) {
            std::cerr << "Warning: Invalid format at line " << line_number << ": '" << line << "'" << std::endl;
            continue;
        }
        
        // Normalize operation
        std::transform(operation.begin(), operation.end(), operation.begin(), ::toupper);
        if (operation == "R" || operation == "READ") {
            operation = "READ";
        } else if (operation == "W" || operation == "WRITE") {
            operation = "WRITE";
        } else {
            std::cerr << "Warning: Unknown operation '" << operation << "' at line " << line_number << std::endl;
            continue;
        }
        
        uint64_t address = parseAddress(address_str);
        if (address == 0 && address_str != "0" && address_str != "0x0") {
            std::cerr << "Warning: Failed to parse address '" << address_str << "' at line " << line_number << std::endl;
            continue;
        }
        
        addresses.push_back(address);
        operations.push_back(operation);
    }
    
    file.close();
    
    if (addresses.empty()) {
        std::cerr << "Error: No valid memory accesses found in trace file '" << filename << "'" << std::endl;
        return false;
    }
    
    std::cout << "Successfully loaded " << addresses.size() << " memory accesses from '" << filename << "'" << std::endl;
    return true;
}

// Function to write statistics to file
bool writeStatsToFile(const std::string& filename, const SetAssociativeCache& cache, const CacheConfig& config, 
                     const std::vector<uint64_t>& addresses, const std::vector<std::string>& operations,
                     const std::vector<Cache::AccessResult>& results, double simulation_time) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create output file '" << filename << "'" << std::endl;
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    file << "========================================\n";
    file << "Cache Simulator Statistics Report\n";
    file << "========================================\n";
    file << "Generated: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n\n";
    
    // Cache configuration
    file << "CACHE CONFIGURATION:\n";
    file << "-------------------\n";
    file << cache.getConfig() << "\n";
    
    // Simulation details
    file << "SIMULATION DETAILS:\n";
    file << "------------------\n";
    file << "Total Memory Accesses: " << addresses.size() << "\n";
    file << "Simulation Time: " << std::fixed << std::setprecision(6) << simulation_time << " seconds\n";
    if (!config.trace_file.empty()) {
        file << "Input Trace File: " << config.trace_file << "\n";
    }
    file << "\n";
    
    // Cache statistics
    file << "CACHE STATISTICS:\n";
    file << "-----------------\n";
    file << cache.getStatistics() << "\n";
    
    // Access details (if verbose or small number of accesses)
    if (config.verbose || addresses.size() <= 100) {
        file << "ACCESS DETAILS:\n";
        file << "--------------\n";
        for (size_t i = 0; i < addresses.size(); ++i) {
            file << std::setw(6) << i + 1 << ": ";
            file << "0x" << std::hex << std::setw(8) << std::setfill('0') << addresses[i] << std::dec << std::setfill(' ');
            file << " (" << std::setw(5) << operations[i] << ") -> ";
            
            switch (results[i]) {
                case Cache::AccessResult::HIT: file << "HIT"; break;
                case Cache::AccessResult::MISS: file << "MISS"; break;
                case Cache::AccessResult::WRITE_HIT: file << "WRITE HIT"; break;
                case Cache::AccessResult::WRITE_MISS: file << "WRITE MISS"; break;
            }
            file << "\n";
        }
        file << "\n";
    }
    
    // Performance summary
    file << "PERFORMANCE SUMMARY:\n";
    file << "-------------------\n";
    auto stats = cache.getStatistics();
    file << "Accesses per second: " << std::fixed << std::setprecision(0) << (addresses.size() / simulation_time) << "\n";
    file << "Average access time: " << std::fixed << std::setprecision(3) << (simulation_time * 1000000 / addresses.size()) << " microseconds\n";
    file << "\n";
    
    file << "========================================\n";
    file << "End of Report\n";
    file << "========================================\n";
    
    file.close();
    return true;
}

CacheConfig parseArguments(int argc, char* argv[]) {
    CacheConfig config;
    
    static struct option long_options[] = {
        {"cache-size",    required_argument, 0, 's'},
        {"block-size",    required_argument, 0, 'b'},
        {"associativity", required_argument, 0, 'a'},
        {"replacement",   required_argument, 0, 'r'},
        {"write-policy",  required_argument, 0, 'w'},
        {"write-miss",    required_argument, 0, 'm'},
        {"trace-file",    required_argument, 0, 't'},
        {"output-file",   required_argument, 0, 'o'},
        {"addresses",     required_argument, 0, 'A'},
        {"operations",    required_argument, 0, 'O'},
        {"interactive",   no_argument,       0, 'i'},
        {"verbose",       no_argument,       0, 'v'},
        {"quiet",         no_argument,       0, 'q'},
        {"help",          no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "s:b:a:r:w:m:t:o:A:O:ivqh", long_options, &option_index)) != -1) {
        switch (c) {
            case 's':
                config.cache_size = std::stoul(optarg);
                break;
            case 'b':
                config.block_size = std::stoul(optarg);
                break;
            case 'a':
                config.associativity = std::stoul(optarg);
                break;
            case 'r':
                config.replacement_policy = optarg;
                std::transform(config.replacement_policy.begin(), 
                             config.replacement_policy.end(),
                             config.replacement_policy.begin(), ::toupper);
                break;
            case 'w':
                config.write_policy = optarg;
                std::transform(config.write_policy.begin(),
                             config.write_policy.end(),
                             config.write_policy.begin(), ::toupper);
                break;
            case 'm':
                config.write_miss_policy = optarg;
                std::transform(config.write_miss_policy.begin(),
                             config.write_miss_policy.end(),
                             config.write_miss_policy.begin(), ::toupper);
                break;
            case 't':
                config.trace_file = optarg;
                break;
            case 'o':
                config.output_file = optarg;
                break;
            case 'A': {
                auto addr_strs = split(optarg, ',');
                for (const auto& addr_str : addr_strs) {
                    config.addresses.push_back(parseAddress(addr_str));
                }
                break;
            }
            case 'O': {
                auto op_strs = split(optarg, ',');
                for (auto op_str : op_strs) {
                    std::transform(op_str.begin(), op_str.end(), op_str.begin(), ::toupper);
                    config.operations.push_back(op_str);
                }
                break;
            }
            case 'i':
                config.interactive = true;
                break;
            case 'v':
                config.verbose = true;
                break;
            case 'q':
                config.quiet = true;
                break;
            case 'h':
                config.help = true;
                break;
            default:
                std::cerr << "Unknown option. Use --help for usage information.\n";
                config.help = true;
                break;
        }
    }
    
    return config;
}

void interactiveMode(SetAssociativeCache& cache, bool verbose) {
    std::cout << "\n=== Interactive Mode ===\n";
    std::cout << "Commands:\n";
    std::cout << "  access <address> <READ|WRITE> - Access memory address\n";
    std::cout << "  batch <addr1,addr2,...> <op1,op2,...> - Batch access\n";
    std::cout << "  stats - Show current statistics\n";
    std::cout << "  reset - Reset cache and statistics\n";
    std::cout << "  config - Show cache configuration\n";
    std::cout << "  contents - Show cache contents\n";
    std::cout << "  help - Show this help\n";
    std::cout << "  quit - Exit interactive mode\n\n";
    
    std::string line;
    while (std::cout << "cache> " && std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (command == "quit" || command == "exit") {
            break;
        } else if (command == "access") {
            std::string addr_str, op_str;
            if (iss >> addr_str >> op_str) {
                uint64_t address = parseAddress(addr_str);
                std::transform(op_str.begin(), op_str.end(), op_str.begin(), ::toupper);
                
                Cache::Operation operation = (op_str == "WRITE") ? 
                    Cache::Operation::WRITE : Cache::Operation::READ;
                
                auto result = cache.access(address, operation);
                
                std::cout << "Access 0x" << std::hex << address << std::dec
                         << " (" << op_str << ") -> ";
                
                switch (result) {
                    case Cache::AccessResult::HIT: std::cout << "HIT"; break;
                    case Cache::AccessResult::MISS: std::cout << "MISS"; break;
                    case Cache::AccessResult::WRITE_HIT: std::cout << "WRITE HIT"; break;
                    case Cache::AccessResult::WRITE_MISS: std::cout << "WRITE MISS"; break;
                }
                std::cout << std::endl;
            } else {
                std::cout << "Usage: access <address> <READ|write>\n";
            }
        } else if (command == "batch") {
            std::string addrs_str, ops_str;
            if (iss >> addrs_str >> ops_str) {
                auto addr_strs = split(addrs_str, ',');
                auto op_strs = split(ops_str, ',');
                
                size_t count = std::min(addr_strs.size(), op_strs.size());
                for (size_t i = 0; i < count; ++i) {
                    uint64_t address = parseAddress(addr_strs[i]);
                    std::string op_str = op_strs[i];
                    std::transform(op_str.begin(), op_str.end(), op_str.begin(), ::toupper);
                    
                    Cache::Operation operation = (op_str == "WRITE") ? 
                        Cache::Operation::WRITE : Cache::Operation::READ;
                    
                    auto result = cache.access(address, operation);
                    
                    std::cout << "Access 0x" << std::hex << address << std::dec
                             << " (" << op_str << ") -> ";
                    
                    switch (result) {
                        case Cache::AccessResult::HIT: std::cout << "HIT"; break;
                        case Cache::AccessResult::MISS: std::cout << "MISS"; break;
                        case Cache::AccessResult::WRITE_HIT: std::cout << "WRITE HIT"; break;
                        case Cache::AccessResult::WRITE_MISS: std::cout << "WRITE MISS"; break;
                    }
                    std::cout << std::endl;
                }
            } else {
                std::cout << "Usage: batch <addr1,addr2,...> <op1,op2,...>\n";
            }
        } else if (command == "stats") {
            std::cout << cache.getStatistics() << std::endl;
        } else if (command == "reset") {
            cache.clear();
            std::cout << "Cache reset successfully.\n";
        } else if (command == "config") {
            std::cout << cache.getConfig() << std::endl;
        } else if (command == "contents") {
            cache.printCacheContents();
        } else if (command == "help") {
            std::cout << "Commands: access, batch, stats, reset, config, contents, help, quit\n";
        } else {
            std::cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }
}

int main(int argc, char* argv[]) {
    CacheConfig config = parseArguments(argc, argv);
    
    if (config.help) {
        printHelp(argv[0]);
        return 0;
    }
    
    try {
        // Validate configuration
        if (config.cache_size == 0 || config.block_size == 0) {
            throw std::invalid_argument("Cache size and block size must be greater than 0");
        }
        
        if (config.cache_size % config.block_size != 0) {
            throw std::invalid_argument("Cache size must be a multiple of block size");
        }
        
        // Create replacement policy
        auto policy_type = ReplacementPolicyFactory::stringToPolicy(config.replacement_policy);
        
        size_t num_sets, actual_associativity;
        if (config.associativity == 0) {
            // Fully associative
            num_sets = 1;
            actual_associativity = config.cache_size / config.block_size;
        } else {
            // Direct mapped or set associative
            actual_associativity = config.associativity;
            num_sets = config.cache_size / (config.block_size * actual_associativity);
        }
        
        auto replacement_policy = ReplacementPolicyFactory::createPolicy(
            policy_type, num_sets, actual_associativity);
        
        // Parse write policies
        SetAssociativeCache::WritePolicy write_policy = 
            (config.write_policy == "WRITE_BACK") ? 
            SetAssociativeCache::WritePolicy::WRITE_BACK :
            SetAssociativeCache::WritePolicy::WRITE_THROUGH;
        
        SetAssociativeCache::WriteMissPolicy write_miss_policy = 
            (config.write_miss_policy == "NO_WRITE_ALLOCATE") ?
            SetAssociativeCache::WriteMissPolicy::NO_WRITE_ALLOCATE :
            SetAssociativeCache::WriteMissPolicy::WRITE_ALLOCATE;
        
        // Create cache
        SetAssociativeCache cache(config.cache_size, config.block_size, 
                                 config.associativity, std::move(replacement_policy),
                                 write_policy, write_miss_policy);
        
        // Display configuration (unless quiet)
        if (!config.quiet) {
            std::cout << "Cache Simulator CLI\n";
            std::cout << "==================\n";
            std::cout << cache.getConfig() << std::endl;
        }
        
        if (config.interactive) {
            interactiveMode(cache, config.verbose);
        } else {
            // Determine input source
            std::vector<uint64_t> addresses;
            std::vector<std::string> operations;
            
            if (!config.trace_file.empty()) {
                // Read from trace file
                if (!config.quiet) {
                    std::cout << "Loading trace file: " << config.trace_file << std::endl;
                }
                if (!readTraceFile(config.trace_file, addresses, operations)) {
                    throw std::runtime_error("Failed to read trace file: " + config.trace_file);
                }
            } else if (!config.addresses.empty()) {
                // Use command line addresses and operations
                addresses = config.addresses;
                operations = config.operations;
            } else {
                // Use default test pattern
                addresses = {0x0, 0x20, 0x40, 0x60, 0x80, 0x100, 0x0, 0x0};
                operations = {"READ", "WRITE", "read", "WRITE", "read", "WRITE", "read", "WRITE"};
                if (!config.quiet) {
                    std::cout << "Using default test pattern.\n\n";
                }
            }
            
            // Ensure operations match addresses
            while (operations.size() < addresses.size()) {
                operations.push_back("READ");
            }
            
            // Record start time for performance measurement
            auto start_time = std::chrono::high_resolution_clock::now();
            
            // Simulate cache accesses
            std::vector<Cache::AccessResult> results;
            results.reserve(addresses.size());
            
            if (!config.quiet) {
                std::cout << "Memory Access Simulation:\n";
                std::cout << "========================\n";
            }
            
            for (size_t i = 0; i < addresses.size(); ++i) {
                std::string op_str = (i < operations.size()) ? operations[i] : "READ";
                std::transform(op_str.begin(), op_str.end(), op_str.begin(), ::toupper);
                
                Cache::Operation operation = (op_str == "WRITE") ? 
                    Cache::Operation::WRITE : Cache::Operation::READ;
                
                Cache::AccessResult result = cache.access(addresses[i], operation);
                results.push_back(result);
                
                // Display progress for console output (unless quiet)
                if (!config.quiet) {
                    if (config.verbose) {
                        std::cout << "Accessing address 0x" << std::hex << addresses[i] 
                                 << std::dec << " with operation " << op_str << " -> ";
                    } else {
                        std::cout << "Access 0x" << std::hex << addresses[i] << std::dec << " (" << op_str << ") -> ";
                    }
                    
                    switch (result) {
                        case Cache::AccessResult::HIT:
                            std::cout << "HIT";
                            break;
                        case Cache::AccessResult::MISS:
                            std::cout << "MISS";
                            break;
                        case Cache::AccessResult::WRITE_HIT:
                            std::cout << "WRITE HIT";
                            break;
                        case Cache::AccessResult::WRITE_MISS:
                            std::cout << "WRITE MISS";
                            break;
                    }
                    std::cout << std::endl;
                }
            }
            
            // Record end time
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            double simulation_time = duration.count() / 1000000.0; // Convert to seconds
            
            // Display statistics to console (unless quiet)
            if (!config.quiet) {
                std::cout << "\n" << cache.getStatistics() << std::endl;
                
                if (config.verbose) {
                    // Print final cache contents
                    cache.printCacheContents();
                }
            }
            
            // Write statistics to file
            if (!config.quiet) {
                std::cout << "\nWriting statistics to " << config.output_file << "..." << std::endl;
            }
            
            if (!writeStatsToFile(config.output_file, cache, config, addresses, operations, results, simulation_time)) {
                throw std::runtime_error("Failed to write statistics to file: " + config.output_file);
            }
            
            if (!config.quiet) {
                std::cout << "Statistics successfully written to " << config.output_file << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
