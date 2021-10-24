
set(SOURCES
    mode_test.cpp
    header_test.cpp
    options_test.cpp
)

set(LIBS
    gtest_main
    TftpProtocol
)

add_executable(TftpProtocolUnitTests ${SOURCES})
target_link_libraries(TftpProtocolUnitTests PRIVATE ${LIBS})
target_include_directories(TftpProtocolUnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(TftpProtocolUnitTests)
