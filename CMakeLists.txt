cmake_minimum_required(VERSION 3.4.3)

project(hpp)
include(CTest)

option(BUILD_HPP_TESTS "Build the tests" ON)

if(BUILD_HPP_TESTS)
	if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	endif()
	if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	endif()
	if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	endif()
endif()

add_subdirectory(hpp)

if(BUILD_HPP_TESTS)
	add_subdirectory(tests)
endif()
