#pragma once
#include "helpers/http.h"
#include <iostream>
#include <string>

/**
 * Example usage of the HttpClient class
 * This class provides a simplified interface for making API requests to specific endpoints
 */
class ApiClient {
public:
    ApiClient(const std::string& baseUrl) : baseUrl(baseUrl) {}

    // Get player data example
    bool getPlayerData(int playerId, std::string& result) {
        std::string endpoint = "/players/" + std::to_string(playerId);
        auto response = httpClient.get(baseUrl + endpoint);

        if (response.isSuccess()) {
            result = response.body;
            return true;
        } else {
            std::cout << "Failed to get player data. Status code: " << response.statusCode << std::endl;
            return false;
        }
    }

    // Update player data example
    bool updatePlayerData(int playerId, const std::string& jsonData, std::string& result) {
        std::string endpoint = "/players/" + std::to_string(playerId);
        auto response = httpClient.put(baseUrl + endpoint, jsonData);

        if (response.isSuccess()) {
            result = response.body;
            return true;
        } else {
            std::cout << "Failed to update player data. Status code: " << response.statusCode << std::endl;
            return false;
        }
    }

    // Create new player data example
    bool createPlayerData(const std::string& jsonData, std::string& result) {
        std::string endpoint = "/players";
        auto response = httpClient.post(baseUrl + endpoint, jsonData);

        if (response.isSuccess()) {
            result = response.body;
            return true;
        } else {
            std::cout << "Failed to create player data. Status code: " << response.statusCode << std::endl;
            return false;
        }
    }

private:
    std::string baseUrl;
    HttpClient httpClient;
};
