#include "helpers/http.h"
#include <curl/curl.h>
#include <iostream>
#include <stdexcept>

// Constructor - Initialize curl
HttpClient::HttpClient() : headersList(nullptr) {
    // Initialize curl global state if it hasn't been initialized yet
    static bool curlInitialized = false;
    if (!curlInitialized) {
        curl_global_init(CURL_GLOBAL_ALL);
        curlInitialized = true;
    }

    curl = std::make_shared<CurlHandle>();
}

// Constructor for CurlHandle
HttpClient::CurlHandle::CurlHandle() {
    handle = curl_easy_init();
    if (!handle) {
        throw std::runtime_error("Failed to initialize curl handle");
    }
}

// Destructor for CurlHandle
HttpClient::CurlHandle::~CurlHandle() {
    if (handle) {
        curl_easy_cleanup(handle);
        handle = nullptr;
    }
}

// Destructor - Clean up resources
HttpClient::~HttpClient() {
    clearHeaders();
}

// Set a request header
HttpClient& HttpClient::setHeader(const std::string& name, const std::string& value) {
    std::string header = name + ": " + value;
    headersList = curl_slist_append(headersList, header.c_str());
    return *this;
}

// Clear all headers
HttpClient& HttpClient::clearHeaders() {
    if (headersList) {
        curl_slist_free_all(headersList);
        headersList = nullptr;
    }
    return *this;
}

// Set timeout for the request
HttpClient& HttpClient::setTimeout(long seconds) {
    curl_easy_setopt(curl->handle, CURLOPT_TIMEOUT, seconds);
    return *this;
}

// Callback function for writing response data
size_t HttpClient::writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t realSize = size * nmemb;
    userp->append(static_cast<char*>(contents), realSize);
    return realSize;
}

// Common setup for requests
void HttpClient::setupRequest(const std::string& url) {
    CURL* handle = curl->handle;

    // Reset handle for new request
    curl_easy_reset(handle);

    // Set URL
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

    // Follow redirects
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);

    // Set default timeout to 30 seconds
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 30L);

    // Set write callback
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCallback);

    // Apply headers if any
    if (headersList) {
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headersList);
    }

    // Always set Content-Type for JSON if not already set
    bool hasContentType = false;
    if (headersList) {
        curl_slist* temp = headersList;
        while (temp) {
            if (strncmp(temp->data, "Content-Type:", 13) == 0) {
                hasContentType = true;
                break;
            }
            temp = temp->next;
        }
    }

    if (!hasContentType) {
        setHeader("Content-Type", "application/json");
        setHeader("Accept", "application/json");
    }
}

// Perform the request and get the response
HttpClient::HttpResponse HttpClient::performRequest() {
    HttpResponse response;

    // Buffer for response data
    std::string responseData;
    curl_easy_setopt(curl->handle, CURLOPT_WRITEDATA, &responseData);

    // Perform the request
    CURLcode res = curl_easy_perform(curl->handle);

    // Check for errors
    if (res != CURLE_OK) {
        std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        response.statusCode = 0;
        response.body = "Failed to perform request: " + std::string(curl_easy_strerror(res));
    } else {
        // Get status code
        curl_easy_getinfo(curl->handle, CURLINFO_RESPONSE_CODE, &response.statusCode);
        response.body = responseData;
    }

    return response;
}

// Make a GET request
HttpClient::HttpResponse HttpClient::get(const std::string& url) {
    setupRequest(url);
    curl_easy_setopt(curl->handle, CURLOPT_HTTPGET, 1L);
    return performRequest();
}

// Make a POST request with JSON data
HttpClient::HttpResponse HttpClient::post(const std::string& url, const std::string& jsonData) {
    setupRequest(url);
    curl_easy_setopt(curl->handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl->handle, CURLOPT_POSTFIELDS, jsonData.c_str());
    return performRequest();
}

// Make a PUT request with JSON data
HttpClient::HttpResponse HttpClient::put(const std::string& url, const std::string& jsonData) {
    setupRequest(url);
    curl_easy_setopt(curl->handle, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl->handle, CURLOPT_POSTFIELDS, jsonData.c_str());
    return performRequest();
}

// Make a DELETE request
HttpClient::HttpResponse HttpClient::del(const std::string& url) {
    setupRequest(url);
    curl_easy_setopt(curl->handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    return performRequest();
}
