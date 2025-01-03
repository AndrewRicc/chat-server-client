# Set minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ChatApplication VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Add library target
add_library(ChatClient
    src/lib/ChatClient/src/ChatClient.cpp
)

# Add include directories for the library
target_include_directories(ChatClient PUBLIC
    src/lib/ChatClient/include
)

# Link threading library
target_link_libraries(ChatClient PUBLIC Threads::Threads)

# Create the chat client executable
add_executable(main_chat_client 
    src/main/main.cpp
)

# Link the chat client with our library
target_link_libraries(main_chat_client PRIVATE ChatClient)