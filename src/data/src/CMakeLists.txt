
set(SOURCES
    file_path.cpp
)

set(LIBS
    TftpProtocol
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)

add_library(TftpData STATIC ${SOURCES})

target_include_directories(TftpData PUBLIC ../include)
target_link_libraries(TftpData PUBLIC ${LIBS})