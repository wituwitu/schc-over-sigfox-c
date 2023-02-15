add_executable(
        CastingTest utils/CastingTest.c
)
add_executable(
        MiscTest utils/MiscTest.c
)
add_executable(
        SocketServerTest sockets/SocketServerTest.c
)
add_executable(
        SocketClientTest sockets/SocketClientTest.c
)
add_executable(
        RuleTest entities/RuleTest.c
)
target_link_libraries(CastingTest utils)
target_link_libraries(MiscTest utils)
target_link_libraries(SocketServerTest sockets)
target_link_libraries(SocketClientTest sockets)
target_link_libraries(RuleTest entities)

###############################################################################
# Unit tests
enable_testing()