project(utils)

set(HEADER_FILES
        casting.h
        )

set(SOURCE_FILES
        casting.c
        )

add_library(utils STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})