set(SOURCES
    file_path.cpp
)

set(LIBS
    TftpProtocol
)

add_library(TftpData STATIC ${SOURCES})

target_include_directories(TftpData PUBLIC ../include)
target_link_libraries(TftpData PUBLIC ${LIBS})