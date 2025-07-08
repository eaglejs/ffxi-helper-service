cmake_minimum_required(VERSION 3.10)

# Set vcpkg toolchain if not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/SL4X3/repos/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Set default triplet for 32-bit
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "x86-windows" CACHE STRING "Vcpkg target triplet")
endif()

project(FFXIHelperService)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(CURL REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/includes)

# Source files
set(SOURCES
    src/FFXIHelperService.cpp
    src/helpers/memory.cpp
    src/helpers/http.cpp
    src/Player/Player.cpp
    src/Player/TacticalPointsProperty.cpp
)

# Header files (optional, for IDE support)
set(HEADERS
    includes/helpers/memory.h
    includes/helpers/http.h
    includes/Player/Player.h
    includes/Player/TacticalPointsProperty.h
)

# Create the executable
add_executable(FFXIHelperService ${SOURCES} ${HEADERS})

# Set compile options
if(MSVC)
    target_compile_options(FFXIHelperService PRIVATE /W4)
else()
    target_compile_options(FFXIHelperService PRIVATE -Wall -Wextra -m32)
    target_link_options(FFXIHelperService PRIVATE -m32)
endif()

# Link libraries
target_link_libraries(FFXIHelperService PRIVATE CURL::libcurl)

# Set debug symbols for Debug builds
set_target_properties(FFXIHelperService PROPERTIES
    DEBUG_POSTFIX "_d"
)
