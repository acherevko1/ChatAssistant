cmake_minimum_required(VERSION 3.20)
project(ChatAssistant)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)

# Source files
add_executable(ChatAssistant
    main.cpp
    telegram.cpp
    bot.cpp
    config.cpp
    openai.cpp
    database.cpp
)

# Link libraries
target_link_libraries(ChatAssistant PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
)

