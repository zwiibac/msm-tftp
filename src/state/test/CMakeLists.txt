find_package(Boost COMPONENTS system REQUIRED)

set(SOURCES
    accepting_test.cpp
    waiting_for_ack_test.cpp
)

set(LIBS
    gtest_main
    gmock
    ${Boost_SYSTEM_LIBRARY}
    TftpState
    TftpEvent
    TftpProtocol
    TestUtils
)

add_executable(TftpStateUnitTests ${SOURCES})
target_link_libraries(TftpStateUnitTests PRIVATE ${LIBS})
target_include_directories(TftpStateUnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(TftpStateUnitTests)
