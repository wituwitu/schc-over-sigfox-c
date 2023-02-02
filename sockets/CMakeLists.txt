project(sockets)

set(HEADER_FILES
        sigfox_socket.h
        )

set(SOURCE_FILES
        sigfox_socket.c
        )

add_library(sockets STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(sockets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
