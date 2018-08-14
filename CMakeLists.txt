cmake_minimum_required(VERSION 3.4.3)

project (hpp)

option(BUILD_HPP_TESTS "Build the tests" OFF)

if(BUILD_HPP_TESTS)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
endif()

add_subdirectory(hpp)

if(BUILD_HPP_TESTS)
	add_subdirectory(tests)
endif()
