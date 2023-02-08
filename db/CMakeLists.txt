project(db)

set(HEADER_FILES
        storage.h
        )

set(SOURCE_FILES
        storage.c
        )

add_library(db STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(db PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})