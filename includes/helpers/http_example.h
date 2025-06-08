/**
 * Example usage of HttpClient
 */
#include "helpers/http.h"
#include <iostream>

void httpClientExample() {
    // Create an HTTP client
    HttpClient client;

    // Set custom headers if needed
    client.setHeader("Authorization", "Bearer your_token_here");

    // Make a GET request
    auto response = client.get("https://api.example.com/data");
    if (response.isSuccess()) {
        std::cout << "GET response: " << response.body << std::endl;
    } else {
        std::cout << "GET request failed with status code: " << response.statusCode << std::endl;
    }

    // Make a POST request with JSON data
    std::string jsonData = R"({
        "name": "FFXI Character",
        "level": 75,
        "job": "Samurai"
    })";

    response = client.post("https://api.example.com/characters", jsonData);
    if (response.isSuccess()) {
        std::cout << "POST response: " << response.body << std::endl;
    } else {
        std::cout << "POST request failed with status code: " << response.statusCode << std::endl;
    }

    // Set a timeout for long requests
    client.setTimeout(60); // 60 seconds timeout

    // Make a PUT request
    jsonData = R"({
        "level": 76,
        "job": "Ninja"
    })";

    response = client.put("https://api.example.com/characters/123", jsonData);
    if (response.isSuccess()) {
        std::cout << "PUT response: " << response.body << std::endl;
    } else {
        std::cout << "PUT request failed with status code: " << response.statusCode << std::endl;
    }
}
