#include "../include/set_associative_cache.h"
#include "../include/replacement_policy.h"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <thread>
#include <regex>

// Simple HTTP server implementation
class CacheServer {
private:
    std::unique_ptr<SetAssociativeCache> cache_;
    int port_;

    // Helper function to parse HTTP request
    std::map<std::string, std::string> parseRequest(const std::string& request) {
        std::map<std::string, std::string> params;
        
        // Extract path and query string
        std::regex pathRegex(R"(GET\s+(/[^\s\?]*)\??([^\s]*)\s+HTTP)");
        std::smatch match;
        
        if (std::regex_search(request, match, pathRegex)) {
            params["path"] = match[1].str();
            std::string queryString = match[2].str();
            
            // Parse query parameters
            std::regex paramRegex(R"(([^&=]+)=([^&]*))");
            std::sregex_iterator iter(queryString.begin(), queryString.end(), paramRegex);
            std::sregex_iterator end;
            
            for (; iter != end; ++iter) {
                params[(*iter)[1].str()] = (*iter)[2].str();
            }
        }
        
        return params;
    }

    // Convert cache result to string
    std::string resultToString(Cache::AccessResult result) {
        switch (result) {
            case Cache::AccessResult::HIT: return "HIT";
            case Cache::AccessResult::MISS: return "MISS";
            case Cache::AccessResult::WRITE_HIT: return "WRITE_HIT";
            case Cache::AccessResult::WRITE_MISS: return "WRITE_MISS";
            default: return "UNKNOWN";
        }
    }

    // Handle cache creation request
    std::string handleCreateCache(const std::map<std::string, std::string>& params) {
        try {
            size_t cache_size = std::stoul(params.at("cache_size"));
            size_t block_size = std::stoul(params.at("block_size"));
            size_t associativity = std::stoul(params.at("associativity"));
            std::string policy_str = params.at("replacement_policy");

            auto policy_type = ReplacementPolicyFactory::stringToPolicy(policy_str);
            
            size_t num_sets = (associativity == 0) ? 1 : cache_size / (block_size * associativity);
            size_t actual_associativity = (associativity == 0) ? cache_size / block_size : associativity;
            
            auto policy = ReplacementPolicyFactory::createPolicy(policy_type, num_sets, actual_associativity);
            
            SetAssociativeCache::WritePolicy write_policy = SetAssociativeCache::WritePolicy::WRITE_THROUGH;
            if (params.count("write_policy") && params.at("write_policy") == "WRITE_BACK") {
                write_policy = SetAssociativeCache::WritePolicy::WRITE_BACK;
            }
            
            SetAssociativeCache::WriteMissPolicy write_miss_policy = SetAssociativeCache::WriteMissPolicy::WRITE_ALLOCATE;
            if (params.count("write_miss_policy") && params.at("write_miss_policy") == "NO_WRITE_ALLOCATE") {
                write_miss_policy = SetAssociativeCache::WriteMissPolicy::NO_WRITE_ALLOCATE;
            }

            cache_ = std::make_unique<SetAssociativeCache>(
                cache_size, block_size, associativity, 
                std::move(policy), write_policy, write_miss_policy
            );

            return R"({"status": "success", "message": "Cache created successfully"})";
        } catch (const std::exception& e) {
            return R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})";
        }
    }

    // Handle cache access request
    std::string handleAccess(const std::map<std::string, std::string>& params) {
        if (!cache_) {
            return R"({"status": "error", "message": "No cache created"})";
        }

        try {
            uint64_t address = std::stoull(params.at("address"), nullptr, 16);
            std::string operation_str = params.at("operation");
            
            Cache::Operation operation = (operation_str == "WRITE") ? 
                Cache::Operation::WRITE : Cache::Operation::READ;
            
            auto result = cache_->access(address, operation);
            auto stats = cache_->getStatistics();
            
            std::ostringstream response;
            response << R"({)"
                     << R"("status": "success",)"
                     << R"("result": ")" << resultToString(result) << R"(",)"
                     << R"("statistics": {)"
                     << R"("total_accesses": )" << stats.getTotalAccesses() << R"(,)"
                     << R"("hits": )" << stats.getHits() << R"(,)"
                     << R"("misses": )" << stats.getMisses() << R"(,)"
                     << R"("hit_rate": )" << stats.getHitRate()
                     << R"(}})";
            
            return response.str();
        } catch (const std::exception& e) {
            return R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})";
        }
    }

    // Handle cache reset request
    std::string handleReset() {
        if (!cache_) {
            return R"({"status": "error", "message": "No cache created"})";
        }
        
        cache_->clear();
        return R"({"status": "success", "message": "Cache reset successfully"})";
    }

    // Handle cache info request
    std::string handleInfo() {
        if (!cache_) {
            return R"({"status": "error", "message": "No cache created"})";
        }
        
        auto stats = cache_->getStatistics();
        std::ostringstream response;
        response << R"({)"
                 << R"("status": "success",)"
                 << R"("cache_size": )" << cache_->getCacheSize() << R"(,)"
                 << R"("block_size": )" << cache_->getBlockSize() << R"(,)"
                 << R"("associativity": )" << cache_->getAssociativity() << R"(,)"
                 << R"("num_sets": )" << cache_->getNumSets() << R"(,)"
                 << R"("statistics": {)"
                 << R"("total_accesses": )" << stats.getTotalAccesses() << R"(,)"
                 << R"("hits": )" << stats.getHits() << R"(,)"
                 << R"("misses": )" << stats.getMisses() << R"(,)"
                 << R"("hit_rate": )" << stats.getHitRate()
                 << R"(}})";
        
        return response.str();
    }

    // Generate HTTP response
    std::string generateResponse(const std::string& content, const std::string& contentType = "application/json") {
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: " << contentType << "\r\n";
        response << "Access-Control-Allow-Origin: *\r\n";
        response << "Content-Length: " << content.length() << "\r\n";
        response << "\r\n";
        response << content;
        return response.str();
    }

public:
    CacheServer(int port) : port_(port) {}

    void start() {
        std::cout << "Cache Simulator Server starting on port " << port_ << std::endl;
        std::cout << "API Endpoints:" << std::endl;
        std::cout << "  GET /create?cache_size=1024&block_size=32&associativity=4&replacement_policy=LRU" << std::endl;
        std::cout << "  GET /access?address=0x1000&operation=READ" << std::endl;
        std::cout << "  GET /reset" << std::endl;
        std::cout << "  GET /info" << std::endl;
        std::cout << "\nServer running... (Press Ctrl+C to stop)" << std::endl;

        // Simple server loop (in production, use proper socket programming)
        while (true) {
            std::string request;
            std::cout << "\nWaiting for request (enter request manually for testing):" << std::endl;
            std::getline(std::cin, request);
            
            if (request.empty()) continue;
            
            auto params = parseRequest(request);
            std::string response_content;
            
            std::string path = params["path"];
            if (path == "/create") {
                response_content = handleCreateCache(params);
            } else if (path == "/access") {
                response_content = handleAccess(params);
            } else if (path == "/reset") {
                response_content = handleReset();
            } else if (path == "/info") {
                response_content = handleInfo();
            } else {
                response_content = R"({"status": "error", "message": "Unknown endpoint"})";
            }
            
            std::cout << "Response: " << generateResponse(response_content) << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }
    
    CacheServer server(port);
    server.start();
    
    return 0;
}
