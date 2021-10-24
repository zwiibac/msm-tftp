set(LIBS
   ${Boost_LOG_LIBRARY}
)

add_library(TftpLogger INTERFACE)

target_include_directories(TftpLogger INTERFACE ../include)
target_link_libraries(TftpLogger INTERFACE ${LIBS})