set(SOURCES
    ctor_test.cpp
    file_path_test.cpp
)

set(LIBS
    gtest_main
    TftpData
)

add_executable(TftpDataUnitTests ${SOURCES})
target_link_libraries(TftpDataUnitTests PRIVATE ${LIBS})
target_include_directories(TftpDataUnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(TftpDataUnitTests)
