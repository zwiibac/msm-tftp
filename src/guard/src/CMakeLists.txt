set(LIBS
   TftpProtocol
)

add_library(TftpGuard INTERFACE)

target_include_directories(TftpGuard INTERFACE ../include)
target_link_libraries(TftpGuard INTERFACE ${LIBS})