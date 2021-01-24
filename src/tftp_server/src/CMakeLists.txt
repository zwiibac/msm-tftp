set(LIBS
    TftpAction
    TftpEvent
    TftpState
    TftpGuard
    TftpMachine
)

add_library(TftpServer INTERFACE)

target_include_directories(TftpServer INTERFACE ../include)
target_link_libraries(TftpServer INTERFACE ${LIBS})
