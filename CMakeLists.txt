cmake_minimum_required(VERSION 3.24)
project(schc_over_sigfox_c C)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_C_STANDARD 17)

###############################################################################
# Subdirectories
add_subdirectory(tests)
add_subdirectory(config)
add_subdirectory(utils)
add_subdirectory(sockets)
add_subdirectory(entities)
add_subdirectory(messages)
