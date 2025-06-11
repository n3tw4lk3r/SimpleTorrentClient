cmake_minimum_required(VERSION 3.16)
project(torrent-client-prototype CXX)

# Build configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-fsanitize=thread)
add_link_options(-fsanitize=thread)

# OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Fetch cpr
include(FetchContent)
set(CPR_USE_SYSTEM_CURL OFF)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba
)
FetchContent_MakeAvailable(cpr)

include_directories(src)

add_library(bencode_parser
    src/BencodeParser.cpp
    src/utils.cpp
)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        bencode_parser
        ${OPENSSL_LIBRARIES}
        cpr::cpr
)
