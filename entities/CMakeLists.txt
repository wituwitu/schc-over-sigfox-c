project(sockets)

set(HEADER_FILES
        rule.h
        fr_procedure.h
        )

set(SOURCE_FILES
        rule.c
        fr_procedure.c
        )

add_library(entities STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(entities config utils messages)