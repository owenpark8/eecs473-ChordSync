set(WEB_SERVER_TARGET web_server)

add_executable(${WEB_SERVER_TARGET})

target_sources(${WEB_SERVER_TARGET} PRIVATE
        src/main.cpp
        src/web.cpp
        src/data.cpp
)

target_include_directories(${WEB_SERVER_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${WEB_SERVER_TARGET} PRIVATE extern)

target_compile_definitions(${WEB_SERVER_TARGET} PRIVATE
    WEB_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/public"
    USER_HOME_DIR="$ENV{HOME}"
)

add_custom_target(run_web_server_scripts ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Compiling tailwind .css file..."
    COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_tailwind.sh ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "Generating source file strings..."
    COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_web_sources.sh ${CMAKE_CURRENT_SOURCE_DIR}/public ${CMAKE_CURRENT_SOURCE_DIR}/include/web_sources.hpp
    COMMENT "Running web server scripts"
)

add_dependencies(${WEB_SERVER_TARGET} run_web_server_scripts)
