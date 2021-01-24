find_package(Boost COMPONENTS system REQUIRED)

set(SOURCES
    form_data_packet_test.cpp
    init_serve_read_request_test.cpp
    time_out_test.cpp
    form_error_packet_test.cpp
    form_ack_packet_test.cpp
    write_data_test.cpp
)

set(LIBS
    gtest_main
    gmock
    ${Boost_SYSTEM_LIBRARY}
    TftpAction
    TestUtils
)

add_executable(TftpActionUnitTests ${SOURCES})
target_link_libraries(TftpActionUnitTests PRIVATE ${LIBS})
target_include_directories(TftpActionUnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(TftpActionUnitTests)
