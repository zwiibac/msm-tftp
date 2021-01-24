set(LIBS
    TftpAction
    TftpEvent
    TftpState
    TftpGuard
)

add_library(TftpMachine INTERFACE)

target_include_directories(TftpMachine INTERFACE ../include)
target_link_libraries(TftpMachine INTERFACE ${LIBS})