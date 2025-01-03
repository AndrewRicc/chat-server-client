# Set minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ChatApplication VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Add library target
add_library(chatlib
    src/ChatClient.cpp
)

# Add include directories for the library
target_include_directories(chatlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link threading library
target_link_libraries(chatlib PUBLIC Threads::Threads)

# Create the chat client executable
add_executable(chat_client 
    src/main.cpp
)

# Link the chat client with our library
target_link_libraries(chat_client PRIVATE chatlib)