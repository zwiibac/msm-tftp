find_package(Boost REQUIRED system)

set(SOURCES
    ctor_test.cpp
    read_request_test.cpp
    base_fixture.cpp
)

set(LIBS
    gtest_main
    gmock
    ${Boost_SYSTEM_LIBRARY}
    TftpMachine
    TftpData
    TestUtils
)

add_executable(TftpMachineUnitTests ${SOURCES})
target_link_libraries(TftpMachineUnitTests PRIVATE ${LIBS})
target_include_directories(TftpMachineUnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(TftpMachineUnitTests)
