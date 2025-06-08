#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>

// Forward declarations to avoid including curl headers in our public interface
struct curl_slist;
typedef void CURL;

/**
 * A class to handle HTTP requests with JSON data
 */
class HttpClient {
public:
    // Constructor/destructor
    HttpClient();
    ~HttpClient();

    /**
     * HTTP response structure
     */
    struct HttpResponse {
        int statusCode;           // HTTP status code
        std::string body;         // Response body
        std::map<std::string, std::string> headers; // Response headers
        bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
    };

    /**
     * Set a request header
     * @param name Header name
     * @param value Header value
     * @return Reference to this for method chaining
     */
    HttpClient& setHeader(const std::string& name, const std::string& value);

    /**
     * Clear all previously set headers
     * @return Reference to this for method chaining
     */
    HttpClient& clearHeaders();

    /**
     * Set timeout for the request in seconds
     * @param seconds Timeout in seconds
     * @return Reference to this for method chaining
     */
    HttpClient& setTimeout(long seconds);

    /**
     * Make a GET request to the specified URL
     * @param url URL to send the request to
     * @return Response object with status code and body
     */
    HttpResponse get(const std::string& url);

    /**
     * Make a POST request with JSON data to the specified URL
     * @param url URL to send the request to
     * @param jsonData JSON data to send in the request body
     * @return Response object with status code and body
     */
    HttpResponse post(const std::string& url, const std::string& jsonData);

    /**
     * Make a PUT request with JSON data to the specified URL
     * @param url URL to send the request to
     * @param jsonData JSON data to send in the request body
     * @return Response object with status code and body
     */
    HttpResponse put(const std::string& url, const std::string& jsonData);

    /**
     * Make a DELETE request to the specified URL
     * @param url URL to send the request to
     * @return Response object with status code and body
     */
    HttpResponse del(const std::string& url);

private:
    // Private implementation details
    struct CurlHandle {
        CURL* handle;
        CurlHandle();
        ~CurlHandle();
    };
    std::shared_ptr<CurlHandle> curl;
    curl_slist* headersList;

    // Common setup for requests
    void setupRequest(const std::string& url);

    // Callback for writing response data
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

    // Perform the request and get the response
    HttpResponse performRequest();
};
