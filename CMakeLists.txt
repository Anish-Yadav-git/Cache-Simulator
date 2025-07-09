cmake_minimum_required(VERSION 3.14)
project(CacheSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")

# Define where to find headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Library source files (excluding main.cpp)
set(CACHE_LIB_SOURCES
    src/cache.cpp
    src/cache_statistics.cpp
    src/replacement_policy.cpp
    src/set_associative_cache.cpp
)

# Create cache library
add_library(cache_lib ${CACHE_LIB_SOURCES})

# Main executable
add_executable(cache_simulator src/main.cpp)
target_link_libraries(cache_simulator cache_lib)

# Test executable
add_executable(cache_test tests/cache_test.cpp)
target_link_libraries(cache_test cache_lib)

# GUI Server executable
add_executable(cache_server gui/cache_server.cpp)
target_link_libraries(cache_server cache_lib)

# Custom targets for running
add_custom_target(run
    COMMAND cache_simulator
    DEPENDS cache_simulator
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(test
    COMMAND cache_test
    DEPENDS cache_test
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(gui
    COMMAND cache_server
    DEPENDS cache_server
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
