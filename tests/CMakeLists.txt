# Google Test setup
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()
include(GoogleTest)

# Test executable
add_executable(cfd_tests
    test_main.cpp
    test_vector3d.cpp
    test_mesh.cpp
    test_field.cpp
    test_geometry.cpp
)

target_link_libraries(cfd_tests
    PRIVATE
        cfd_engine_lib
        gtest
        gtest_main
)

# Discover tests
gtest_discover_tests(cfd_tests)

# Add custom target to run tests
add_custom_target(run_tests
    COMMAND cfd_tests
    DEPENDS cfd_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
