project(utils)

set(HEADER_FILES
        casting.h
        misc.h
        )

set(SOURCE_FILES
        casting.c
        misc.c
        ../get_bin.c)

add_library(utils STATIC ${SOURCE_FILES} ${HEADER_FILES} ../get_bin.c)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(utils PRIVATE m)