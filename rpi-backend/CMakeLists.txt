cmake_minimum_required(VERSION 3.21)

project(eecs473-rpi-backend)

set(CMAKE_CXX_STANDARD 11)
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
)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(WEB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/public)


target_compile_definitions(${RPI_BACKEND_TARGET} PRIVATE
    WEB_SRC_DIR="${WEB_SRC_DIR}"
    USER_HOME_DIR="$ENV{HOME}"
)

# option(ENABLE_WEB_SERVER_SCRIPTS "Enable running web server scripts during the build" ON)

if (ENABLE_WEB_SERVER_SCRIPTS)
    set(COMPILED_CSS_FILE ${WEB_SRC_DIR}/compiled.css)
    set(WEB_SOURCES_FILE ${INCLUDE_DIR}/web_sources.hpp)

    # Custom target to compile Tailwind CSS
    add_custom_target(
        compile_css ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Compiling Tailwind .css file..."
        COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_tailwind.sh ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating compiled.css with Tailwind"
        VERBATIM
    )

    # Custom target to generate web sources
    add_custom_target(
        generate_web_sources ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Generating source file strings..."
        COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_web_sources.sh ${CMAKE_CURRENT_SOURCE_DIR}/public ${WEB_SOURCES_FILE}
        DEPENDS compile_css
        COMMENT "Generating web_sources.hpp"
        VERBATIM
    )

    # Add the generated file as a source for the target
    add_dependencies(${RPI_BACKEND_TARGET} generate_web_sources)
    target_sources(${RPI_BACKEND_TARGET} PRIVATE ${WEB_SOURCES_FILE})
endif()

target_include_directories(${RPI_BACKEND_TARGET} PUBLIC ${INCLUDE_DIR})

# external dependencies
add_subdirectory(extern)
target_link_libraries(${RPI_BACKEND_TARGET} PRIVATE extern)

# pybind11
set(PYTHON_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/py")
set(PYTHON_VENV_PATH "${PYTHON_MODULE_PATH}/.venv/lib/python3.11/site-packages")
target_compile_definitions(${RPI_BACKEND_TARGET} PRIVATE
    PY_MODULE_PATH="${PYTHON_MODULE_PATH}"
    PY_VENV_PATH="${PYTHON_VENV_PATH}"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()
