#include <iostream>
#include <string>
#include <curl/curl.h>
#include <thread>
#include <chrono>

// --- HARDCODED CONFIGURATION ---
const std::string UPSTASH_URL = "https://causal-marmoset-240640.upstash.io";
// PASTE YOUR TOKEN HERE
const std::string UPSTASH_TOKEN = "gQAAAAAAA6wAAAIgcDI1Y2QxYjc2MTJiMGU0OGM1YjlkMGU1MjU3NzU4ZDEyYg";

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

    if (UPSTASH_TOKEN == "gQAAAAAAA6wAAAIgcDI1Y2QxYjc2MTJiMGU0OGM1YjlkMGU1MjU3NzU4ZDEyYg" || UPSTASH_TOKEN.empty()) {
        std::cerr << "[FATAL] You must hardcode your Upstash token in the C++ file!" << std::endl;
        return 1;
    }

    int cycle = 0;
    while (true) {
        cycle++;
        
        // 1. SEND HEARTBEAT
        std::string heartbeat = "{\"node\": \"cpp-node-fly\", \"cycle\": " + std::to_string(cycle) + ", \"status\": \"online\"}";
        upstashRequest(UPSTASH_URL + "/set/proteus:heartbeat", "POST", heartbeat);
        std::cout << "[C++ NODE] Heartbeat sent to Upstash. Cycle: " << cycle << std::endl;

        // 2. CHECK FOR COMMANDS
        std::string response = upstashRequest(UPSTASH_URL + "/get/proteus:command", "GET");
        
        std::cout << "[DEBUG] Upstash GET response: " << response << std::endl;
        
        if (response.find("\"result\":") != std::string::npos && response.find("null") == std::string::npos) {
            std::cout << "\n============================================\n";
            std::cout << "[C++ NODE] 🔥 RECEIVED GLOBAL COMMAND:\n" << response << "\n";
            std::cout << "============================================\n\n";
            
            upstashRequest(UPSTASH_URL + "/del/proteus:command", "POST");
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    curl_global_cleanup();
    return 0;
}
