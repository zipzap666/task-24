include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(
    logger Logger.h
)

add_library(
    settings SettingsManagement.h
)