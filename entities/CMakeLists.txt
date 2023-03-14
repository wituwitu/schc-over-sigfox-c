project(sockets)

set(HEADER_FILES
        rule.h
        fr_procedure.h
        fifo_queue.h
        schc_sender.h
        schc_session.h
        schc_receiver.h
        )

set(SOURCE_FILES
        rule.c
        fr_procedure.c
        fifo_queue.c
        schc_sender.c
        schc_session.c
        schc_receiver.c
        )

add_library(entities STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(entities config utils messages sockets m)