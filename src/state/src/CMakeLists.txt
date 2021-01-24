set(LIBS
    TftpLogger
    TftpProtocol
    TftpEvent
)

add_library(TftpState INTERFACE)

target_include_directories(TftpState INTERFACE ../include)
target_link_libraries(TftpState INTERFACE ${LIBS})