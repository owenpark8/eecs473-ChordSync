add_subdirectory(httplib)
add_subdirectory(SQLiteCpp)
add_subdirectory(wiringPi)
add_subdirectory(fmt)
add_subdirectory(ctre)
add_subdirectory(pybind11)

add_library(extern INTERFACE)
target_link_libraries(extern INTERFACE
    httplib
    SQLiteCpp
    wiringPi
    fmt
    ctre
    pybind11::embed
)
