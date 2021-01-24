set(LIBS
    TftpLogger
    TftpProtocol
)

add_library(TftpAction INTERFACE)

target_include_directories(TftpAction INTERFACE ../include)
target_link_libraries(TftpAction INTERFACE ${LIBS})