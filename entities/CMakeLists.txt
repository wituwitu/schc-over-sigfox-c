project(sockets)

set(HEADER_FILES
        rule.h
        sigfoxprofile.h
        )

set(SOURCE_FILES
        rule.c
        sigfoxprofile.c
        )

add_library(entities STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(entities config)
target_link_libraries(entities utils)