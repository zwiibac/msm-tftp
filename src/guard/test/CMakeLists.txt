
set(SOURCES
    can_retry_test.cpp
    has_ack_received_test.cpp
    has_data_received_test.cpp
    from_agreed_endpoint_test.cpp
    logical_operators_test.cpp
    is_old_ack_test.cpp
    is_old_data_test.cpp
    has_read_request_received_test.cpp
    has_write_request_received_test.cpp
)

set(LIBS
    gtest_main
    TftpGuard
)

add_executable(TftpGuardUnitTests ${SOURCES})
target_link_libraries(TftpGuardUnitTests PRIVATE ${LIBS})
target_include_directories(TftpGuardUnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(TftpGuardUnitTests)
