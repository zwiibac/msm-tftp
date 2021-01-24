set(SOURCES
    ctor_test.cpp
    file_path_test.cpp
)

set(LIBS
    gtest_main
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    TftpData
)

add_executable(TftpDataUnitTests ${SOURCES})
target_link_libraries(TftpDataUnitTests PRIVATE ${LIBS})
target_include_directories(TftpDataUnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GoogleTest)
gtest_discover_tests(TftpDataUnitTests)
