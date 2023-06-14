include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

set(SERVER_SOURCES
        main.cpp
        server.cpp
        server.h
)

add_executable(server ${SERVER_SOURCES})