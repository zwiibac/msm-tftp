set(LIBS
   TftpProtocol
)

add_library(TftpEvent INTERFACE)

target_include_directories(TftpEvent INTERFACE ../include)
target_link_libraries(TftpEvent INTERFACE ${LIBS})