cmake_minimum_required(VERSION 3.25.1)

project(eecs473-rpi-backend)

# C++ Standard and other build configurations
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # IDE support
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(BUILD_SHARED_LIBS OFF) # STATIC

# Target definition
set(PYBIND_TEST_TARGET pybind-test)

add_executable(${PYBIND_TEST_TARGET})

target_sources(${PYBIND_TEST_TARGET} PRIVATE
    src/main.cpp
    src/playerMode.cpp
)

target_include_directories(${PYBIND_TEST_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Python Virtual Environment Setup
set(PYTHON_EXECUTABLE "python3")
set(VENV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/py/.venv")
set(REQUIREMENTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/py/requirements.txt")

# Create virtual environment if it doesn't exist
if(NOT EXISTS "${VENV_DIR}/bin/activate")
    message(STATUS "Creating Python virtual environment in ${VENV_DIR}")
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -m venv ${VENV_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

# Install Python requirements
if(EXISTS "${REQUIREMENTS_FILE}")
    message(STATUS "Installing Python dependencies from ${REQUIREMENTS_FILE}")
    execute_process(
        COMMAND ${VENV_DIR}/bin/python -m pip install --upgrade pip
        COMMAND ${VENV_DIR}/bin/python -m pip install -r ${REQUIREMENTS_FILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else()
    message(WARNING "Requirements file not found: ${REQUIREMENTS_FILE}")
endif()


 
# pybind11
set(PYTHON_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/py")

# Setting Python paths for the target
set(PYTHON_VENV_PATH "${PYTHON_MODULE_PATH}/.venv/lib/python3.11/site-packages")

target_compile_definitions(${PYBIND_TEST_TARGET} PRIVATE
    PY_MODULE_PATH="${PYTHON_MODULE_PATH}"
    PY_VENV_PATH="${PYTHON_VENV_PATH}"
)

# External libraries
add_subdirectory(extern)
target_link_libraries(${PYBIND_TEST_TARGET} PRIVATE extern)

# Debug definitions
add_compile_definitions(DEBUG)
