include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
    test_main.cpp
    test_session.cpp
    test_cipher.cpp
    test_compress.cpp
    test_fragment.cpp
    test_protocol.cpp
    test_queue.cpp
    test_routing.cpp
    test_parser.cpp
)

add_executable(aeroframe_tests ${TEST_SOURCES})
target_link_libraries(aeroframe_tests PRIVATE aeroframe gtest_main)
target_include_directories(aeroframe_tests PRIVATE ${gtest_SOURCE_DIR}/include)

add_test(NAME aeroframe COMMAND aeroframe_tests)
