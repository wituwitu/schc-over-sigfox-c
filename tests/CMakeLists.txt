add_executable(
        CastingTest CastingTest.c
)
add_executable(
        MiscTest MiscTest.c
)
add_executable(
        SocketServerTest SocketServerTest.c
)
add_executable(
        SocketClientTest SocketClientTest.c
)
target_link_libraries(CastingTest utils casting)
target_link_libraries(MiscTest utils casting)
target_link_libraries(SocketServerTest sockets)
target_link_libraries(SocketClientTest sockets)

###############################################################################
# Unit tests
enable_testing()