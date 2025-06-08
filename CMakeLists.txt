cmake_minimum_required(VERSION 3.10)

project(FFXIHelperService)

find_package(CURL REQUIRED)

add_executable(FFXIHelperService src/FFXIHelperService.cpp)

target_link_libraries(FFXIHelperService PRIVATE CURL::libcurl)
