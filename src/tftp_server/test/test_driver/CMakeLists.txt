set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set( SOURCES
    server_driver.cpp
)

set( LIBS
    Threads::Threads
    TftpServer
    TftpData
    TftpLogger
)

add_executable(TftpServerTestDriver ${SOURCES})
target_link_libraries(TftpServerTestDriver PRIVATE ${LIBS})

add_custom_command(
        TARGET TftpServerTestDriver POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/pytest
                ${CMAKE_CURRENT_BINARY_DIR}/pytest)

add_test(NAME ServerIntegrationTest 
    COMMAND ${PYTHON_EXECUTABLE} -m pytest pytest
    WORKING_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY})

#COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/server_test.py --verbose 
