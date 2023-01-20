add_executable(CastingTest CastingTest.c)
target_link_libraries(CastingTest utils)

###############################################################################
# Unit tests
enable_testing()
add_test(NAME check_schc COMMAND check_schc)