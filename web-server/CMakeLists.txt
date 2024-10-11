set(WEB_SERVER_TARGET web_server)

add_executable(${WEB_SERVER_TARGET})

target_sources(${WEB_SERVER_TARGET} PRIVATE
        src/main.cpp
        src/web.cpp
)

target_include_directories(${WEB_SERVER_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/web-server/include)

target_link_libraries(${WEB_SERVER_TARGET} PRIVATE extern)

target_compile_definitions(${WEB_SERVER_TARGET} PRIVATE
    WEB_SRC_DIR="${CMAKE_SOURCE_DIR}/web-server/public"
    USER_HOME_DIR="$ENV{HOME}"
)
