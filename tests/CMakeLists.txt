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
add_executable(
        FRProcedureTest entities/FRProcedureTest.c
)
add_executable(
        SCHCSenderTest entities/SCHCSenderTest.c
)
add_executable(
        SCHCSenderTestRecv entities/SCHCSenderTestRecv.c
)
add_executable(
        FragmentTest messages/FragmentTest.c
)
add_executable(
        CompoundACKTest messages/CompoundACKTest.c
)

target_link_libraries(CastingTest utils)
target_link_libraries(MiscTest utils)
target_link_libraries(SocketServerTest sockets)
target_link_libraries(SocketClientTest sockets)
target_link_libraries(RuleTest entities)
target_link_libraries(FRProcedureTest entities)
target_link_libraries(SCHCSenderTest entities)
target_link_libraries(SCHCSenderTestRecv entities)
target_link_libraries(FragmentTest messages)
target_link_libraries(CompoundACKTest messages)

###############################################################################
# Unit tests
enable_testing()