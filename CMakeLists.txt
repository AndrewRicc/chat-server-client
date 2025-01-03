cmake_minimum_required(VERSION 3.10)

project(ChatApplication VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific configurations
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(PLATFORM_LIBS ws2_32)
else()
    set(PLATFORM_LIBS "")
endif()

find_package(Threads REQUIRED)

# Add library target
add_library(ChatClient
    src/lib/ChatClient/src/ChatClient.cpp
)

# Add include directories for the library
target_include_directories(ChatClient PUBLIC
    src/lib/ChatClient/include
)

# Link platform-specific libraries
target_link_libraries(ChatClient 
    PUBLIC 
        Threads::Threads
        ${PLATFORM_LIBS}
)

# Create the chat client executable
add_executable(main_chat_client 
    src/main/client_chat.cpp
)

# Link the chat client with our library
target_link_libraries(main_chat_client 
    PRIVATE 
        ChatClient
)