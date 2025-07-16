#include "../../include/set_associative_cache.h"
#include "../../include/replacement_policy.h"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <thread>
#include <regex>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <signal.h>
#endif

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

    // URL decode function
    std::string urlDecode(const std::string& str) {
        std::string decoded;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '%' && i + 2 < str.length()) {
                int value = 0;
                std::sscanf(str.substr(i + 1, 2).c_str(), "%x", &value);
                decoded += static_cast<char>(value);
                i += 2;
            } else if (str[i] == '+') {
                decoded += ' ';
            } else {
                decoded += str[i];
            }
        }
        return decoded;
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

    // Handle cache contents request
    std::string handleContents() {
        if (!cache_) {
            return R"({"status": "error", "message": "No cache created"})";
        }
        
        std::ostringstream response;
        response << R"({)"
                 << R"("status": "success",)"
                 << R"("cache_size": )" << cache_->getCacheSize() << R"(,)"
                 << R"("block_size": )" << cache_->getBlockSize() << R"(,)"
                 << R"("associativity": )" << cache_->getAssociativity() << R"(,)"
                 << R"("num_sets": )" << cache_->getNumSets() << R"(,)"
                 << R"("contents": )" << generateCacheContentsJSON()
                 << R"(})";
        
        return response.str();
    }

    // Generate detailed cache contents JSON
    std::string generateCacheContentsJSON() {
        if (!cache_) return "[]";
        
        std::ostringstream json;
        json << "[";
        
        size_t numSets = cache_->getNumSets();
        size_t associativity = cache_->getAssociativity();
        
        for (size_t setIndex = 0; setIndex < numSets; ++setIndex) {
            if (setIndex > 0) json << ",";
            json << "{";
            json << R"("set": )" << setIndex << ",";
            json << R"("blocks": [)";
            
            for (size_t blockIndex = 0; blockIndex < associativity; ++blockIndex) {
                if (blockIndex > 0) json << ",";
                json << "{";
                json << R"("block": )" << blockIndex << ",";
                json << R"("valid": )" << (cache_->isBlockValid(setIndex, blockIndex) ? "true" : "false") << ",";
                json << R"("dirty": )" << (cache_->isBlockDirty(setIndex, blockIndex) ? "true" : "false") << ",";
                json << R"("tag": ")" << std::hex << cache_->getBlockTag(setIndex, blockIndex) << std::dec << "\"";
                json << "}";
            }
            
            json << "]";
            json << "}";
        }
        
        json << "]";
        return json.str();
    }

    // Handle trace file processing
    std::string handleTraceFile(const std::map<std::string, std::string>& params) {
        if (!cache_) {
            return R"({"status": "error", "message": "No cache created"})";
        }
        
        if (params.find("trace_data") == params.end()) {
            return R"({"status": "error", "message": "No trace data provided"})";
        }
        
        std::string traceData = urlDecode(params.at("trace_data"));
        std::vector<std::string> results;
        
        std::istringstream iss(traceData);
        std::string line;
        
        while (std::getline(iss, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream lineStream(line);
            std::string operation;
            std::string addressStr;
            
            if (lineStream >> operation >> addressStr) {
                try {
                    uint64_t address = std::stoull(addressStr, nullptr, 16);
                    Cache::Operation op = (operation == "W" || operation == "WRITE") ? 
                        Cache::Operation::WRITE : Cache::Operation::READ;
                    
                    auto result = cache_->access(address, op);
                    
                    std::ostringstream resultJson;
                    resultJson << "{";
                    resultJson << R"("address": ")" << addressStr << "\",";
                    resultJson << R"("operation": ")" << operation << "\",";
                    resultJson << R"("result": ")" << resultToString(result) << "\"";
                    resultJson << "}";
                    
                    results.push_back(resultJson.str());
                } catch (const std::exception& e) {
                    std::ostringstream errorJson;
                    errorJson << "{";
                    errorJson << R"("address": ")" << addressStr << "\",";
                    errorJson << R"("operation": ")" << operation << "\",";
                    errorJson << R"("result": "ERROR",)";
                    errorJson << R"("error": ")" << e.what() << "\"";
                    errorJson << "}";
                    
                    results.push_back(errorJson.str());
                }
            }
        }
        
        auto stats = cache_->getStatistics();
        
        std::ostringstream response;
        response << "{";
        response << R"("status": "success",)";
        response << R"("processed_count": )" << results.size() << ",";
        response << R"("results": [)";
        
        for (size_t i = 0; i < results.size(); ++i) {
            if (i > 0) response << ",";
            response << results[i];
        }
        
        response << "],";
        response << R"("statistics": {)";
        response << R"("total_accesses": )" << stats.getTotalAccesses() << ",";
        response << R"("hits": )" << stats.getHits() << ",";
        response << R"("misses": )" << stats.getMisses() << ",";
        response << R"("hit_rate": )" << stats.getHitRate();
        response << "}";
        response << "}";
        
        return response.str();
    }

    // Trim whitespace from string
    std::string trim(const std::string& str) {
        auto start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        
        auto end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
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

    // Handle client request
    void handleClient(int client_socket) {
        char buffer[4096];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
#ifdef _WIN32
            closesocket(client_socket);
#else
            close(client_socket);
#endif
            return;
        }
        
        buffer[bytes_received] = '\0';
        std::string request(buffer);
        
        // Debug: print request
        std::cout << "Received request: " << request.substr(0, 100) << std::endl;
        
        auto params = parseRequest(request);
        std::string response_content;
        
        std::string path = params["path"];
        std::cout << "Parsed path: " << path << std::endl;
        
        if (path == "/create") {
            response_content = handleCreateCache(params);
        } else if (path == "/access") {
            response_content = handleAccess(params);
        } else if (path == "/reset") {
            response_content = handleReset();
        } else if (path == "/info") {
            response_content = handleInfo();
        } else if (path == "/contents") {
            response_content = handleContents();
        } else if (path == "/trace") {
            response_content = handleTraceFile(params);
        } else {
            response_content = R"({"status": "error", "message": "Unknown endpoint"})";
        }
        
        std::cout << "Response content: " << response_content.substr(0, 200) << std::endl;
        
        std::string response = generateResponse(response_content);
        send(client_socket, response.c_str(), response.length(), 0);
        
#ifdef _WIN32
        closesocket(client_socket);
#else
        close(client_socket);
#endif
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
        std::cout << "  GET /contents" << std::endl;
        std::cout << "  GET /trace?trace_data=<trace_content>" << std::endl;
        std::cout << "\nServer running... (Press Ctrl+C to stop)" << std::endl;

#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            std::cerr << "Error creating socket" << std::endl;
            return;
        }

        int opt = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port_);

        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            std::cerr << "Error binding socket" << std::endl;
#ifdef _WIN32
            closesocket(server_socket);
#else
            close(server_socket);
#endif
            return;
        }

        if (listen(server_socket, 10) == -1) {
            std::cerr << "Error listening on socket" << std::endl;
#ifdef _WIN32
            closesocket(server_socket);
#else
            close(server_socket);
#endif
            return;
        }

        std::cout << "Server listening on port " << port_ << std::endl;

        while (true) {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_socket != -1) {
                // Handle client in a separate thread
                std::thread client_thread(&CacheServer::handleClient, this, client_socket);
                client_thread.detach();
            }
        }

#ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
#else
        close(server_socket);
#endif
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
