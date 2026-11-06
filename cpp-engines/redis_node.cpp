#include <iostream>
#include <string>
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <cstdlib> // Required for std::getenv

// --- RAILWAY ENVIRONMENT CONFIGURATION ---
// These will be read from the variables you set in the Railway dashboard.
const std::string UPSTASH_URL = std::getenv("https://causal-marmoset-240640.upstash.io") ? std::getenv("https://causal-marmoset-240640.upstash.io") : "";
const std::string UPSTASH_TOKEN = std::getenv("gQAAAAAAA6wAAAIgcDI1Y2QxYjc2MTJiMGU0OGM1YjlkMGU1MjU3NzU4ZDEyYg") ? std::getenv("gQAAAAAAA6wAAAIgcDI1Y2QxYjc2MTJiMGU0OGM1YjlkMGU1MjU3NzU4ZDEyYg") : "";

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string upstashRequest(const std::string& url, const std::string& method, const std::string& body = "") {
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    if(curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + UPSTASH_TOKEN).c_str());
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }
        
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    return readBuffer;
}

int main() {
    curl_global_init(CURL_GLOBAL_ALL);
    std::cout << "[C++ NODE] Connecting to Global Upstash Bus..." << std::endl;

    // Safety check
    if (UPSTASH_URL.empty() || UPSTASH_TOKEN.empty()) {
        std::cerr << "[FATAL] Missing UPSTASH_URL or UPSTASH_TOKEN environment variables!" << std::endl;
        return 1;
    }

    int cycle = 0;
    while (true) {
        cycle++;
        
        // 1. SEND HEARTBEAT
        std::string heartbeat = "{\"node\": \"cpp-node-railway\", \"cycle\": " + std::to_string(cycle) + ", \"status\": \"online\"}";
        upstashRequest(UPSTASH_URL + "/set/proteus:heartbeat", "POST", heartbeat);
        std::cout << "[C++ NODE] Heartbeat sent to Upstash. Cycle: " << cycle << std::endl;

        // 2. CHECK FOR COMMANDS
        std::string response = upstashRequest(UPSTASH_URL + "/get/proteus:command", "GET");
        
        // DEBUG: See exactly what Upstash is returning
        std::cout << "[DEBUG] Upstash GET response: " << response << std::endl;
        
        // If the response contains a result that isn't null, we have a command!
        if (response.find("\"result\":") != std::string::npos && response.find("null") == std::string::npos) {
            std::cout << "\n============================================\n";
            std::cout << "[C++ NODE] 🔥 RECEIVED GLOBAL COMMAND:\n" << response << "\n";
            std::cout << "============================================\n\n";
            
            // Delete the command so we don't run it twice
            upstashRequest(UPSTASH_URL + "/del/proteus:command", "POST");
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    curl_global_cleanup();
    return 0;
}
