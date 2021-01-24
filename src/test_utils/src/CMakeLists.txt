set(LIBS
    TftpProtocol
)

add_library(TestUtils INTERFACE)

target_include_directories(TestUtils INTERFACE ../include)
target_link_libraries(TestUtils INTERFACE ${LIBS})