project(config)

set(HEADER_FILES
        schc.h
        )

set(SOURCE_FILES
        schc.c
        )

add_library(config STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
