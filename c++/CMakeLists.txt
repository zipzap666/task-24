cmake_minimum_required(VERSION 3.14)

project(server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Core REQUIRED)
find_package(QT NAMES Qt5 COMPONENTS Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/common/proto)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/common/parser)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/common)

set(QT_LIBS
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

set(PROJECT_LIBS
    settings
    logger
    proto
    parser
)

target_link_libraries(parser PUBLIC proto)
target_link_libraries(logger PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(settings PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(server PUBLIC ${PROJECT_LIBS})
target_link_libraries(server PRIVATE ${QT_LIBS})