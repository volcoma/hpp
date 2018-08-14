message(STATUS "Enabled tests.")

set(target_name hpp_tests)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable(${target_name} ${libsrc})

target_link_libraries(${target_name} PUBLIC hpp)

set_target_properties(${target_name} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
