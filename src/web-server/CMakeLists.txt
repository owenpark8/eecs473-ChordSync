set(WEB_SERVER_TARGET web-server)

add_executable(${WEB_SERVER_TARGET})

target_sources(${WEB_SERVER_TARGET} PRIVATE
        main.cpp
        web.cpp
)

target_include_directories(${WEB_SERVER_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/inc)

target_link_libraries(${WEB_SERVER_TARGET} PRIVATE extern)

target_compile_definitions(${WEB_SERVER_TARGET} PRIVATE
    WEB_SRC_DIR="${CMAKE_SOURCE_DIR}/src/web-server/public"
    USER_HOME_DIR="$ENV{HOME}"
)
