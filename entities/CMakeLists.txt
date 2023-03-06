project(sockets)

set(HEADER_FILES
        rule.h
        fr_procedure.h
        schc_sender.h
        fifo_queue.h
        )

set(SOURCE_FILES
        rule.c
        fr_procedure.c
        schc_sender.c
        fifo_queue.c
        )

add_library(entities STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(entities config utils messages sockets m)