cmake_minimum_required(VERSION 3.16)
project(DynamicHashTables LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==================== LIBRARY ====================
add_library(HashLib STATIC
    src/LinearHashTable.cpp
    src/ExtendibleHashTable.cpp
    src/LarsonDynamicHashTable.cpp
)

target_include_directories(HashLib PUBLIC include)

# ==================== MAIN APP ====================
add_executable(App main.cpp)
target_link_libraries(App PRIVATE HashLib)

# ==================== TEST APP ====================
add_executable(HashTest tests/test_main.cpp)
target_link_libraries(HashTest PRIVATE HashLib)

message(STATUS "✅ Project configured successfully!")