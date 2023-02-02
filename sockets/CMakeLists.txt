project(utils)

set(HEADER_FILES
        sigfox_socket.h
        )

set(SOURCE_FILES
        sigfox_socket.c
        )

add_library(socket STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(socket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
