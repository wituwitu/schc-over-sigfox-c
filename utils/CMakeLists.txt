project(utils)

set(HEADER_FILES
        casting.h
        misc.h
        )

set(SOURCE_FILES
        casting.c
        misc.c
        ../config/schc.c ../config/schc.h ../sockets/sigfox_http_socket.c ../sockets/sigfox_http_socket.h ../sockets/server.c ../sockets/client.c)

add_library(utils STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(utils PRIVATE m)