set( SOURCES
    error_code.cpp
    mode.cpp
    opcode.cpp
    tid.cpp
    header.cpp
)

add_library(TftpProtocol STATIC ${SOURCES})

set_target_properties(TftpProtocol PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(TftpProtocol PUBLIC ../include)