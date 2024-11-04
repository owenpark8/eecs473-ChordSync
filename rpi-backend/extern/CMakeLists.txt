add_subdirectory(httplib)
add_subdirectory(SQLiteCpp)
add_subdirectory(wiringPi)

add_library(extern INTERFACE)
target_link_libraries(extern INTERFACE
    httplib
    SQLiteCpp
    wiringPi
)
