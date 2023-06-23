cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Interpreter)

#======================= INCLUSION OF Our Code ======================#
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/tests")

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

add_subdirectory(${TEST_DIR})

file(GLOB_RECURSE SOURCES
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

add_library(InterpreterLib ${SOURCES})
list(REMOVE_ITEM InterpreterLib "${SOURCES}/main.cpp")

# add the data to the target
add_executable(Interpreter "${SOURCE_DIR}/main.cpp")
target_link_libraries(Interpreter InterpreterLib)

# Add the target includes
target_include_directories(Interpreter PRIVATE ${INCLUDE_DIR})
target_include_directories(Interpreter PRIVATE ${SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Interpreter)