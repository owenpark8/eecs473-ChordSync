cmake_minimum_required(VERSION 3.21)

project(eecs473-rpi-backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # IDE support
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(BUILD_SHARED_LIBS OFF) # STATIC

set(RPI_BACKEND_TARGET rpi-backend)
add_executable(${RPI_BACKEND_TARGET})

target_sources(${RPI_BACKEND_TARGET} PRIVATE
    src/main.cpp
    src/web.cpp
    src/data.cpp
    src/mcu.cpp
    src/serial.cpp
)

target_include_directories(${RPI_BACKEND_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(extern)
target_link_libraries(${RPI_BACKEND_TARGET} PRIVATE extern)

target_compile_definitions(${RPI_BACKEND_TARGET} PRIVATE
    WEB_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/public/"
    USER_HOME_DIR="$ENV{HOME}/"
)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_target(run_web_server_scripts ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Compiling tailwind .css file..."
            COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_tailwind.sh ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${CMAKE_COMMAND} -E echo "Generating source file strings..."
            COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_web_sources.sh ${CMAKE_CURRENT_SOURCE_DIR}/public ${CMAKE_CURRENT_SOURCE_DIR}/include/web_sources.hpp
            COMMENT "Running web server scripts"
    )
    add_dependencies(${RPI_BACKEND_TARGET} run_web_server_scripts)
else()
    add_compile_definitions(DEBUG)
endif()