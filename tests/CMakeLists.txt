set(TEST_DIR "${CMAKE_SOURCE_DIR}/tests")

file(GLOB_RECURSE SOURCES
    "${TEST_DIR}/*.hpp"
    "${TEST_DIR}/*.cpp"
)

#set(SOURCE_FILES
#	catch.hpp
#	tests.cpp
#)

add_executable(UnitTests ${SOURCES})
target_link_libraries(UnitTests InterpreterLib)
