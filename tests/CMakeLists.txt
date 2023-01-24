add_executable(
        CastingTest CastingTest.c
)
add_executable(
        MiscTest MiscTest.c
)
target_link_libraries(CastingTest utils)
target_link_libraries(MiscTest utils)

###############################################################################
# Unit tests
enable_testing()
add_test(NAME check_schc COMMAND check_schc)