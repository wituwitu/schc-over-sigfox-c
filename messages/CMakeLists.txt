project(messages)

set(HEADER_FILES
        fragment.h
        )

set(SOURCE_FILES
        fragment.c )

add_library(messages STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(messages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(messages entities utils config)