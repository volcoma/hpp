set(target_name hpp)

add_library(${target_name} INTERFACE)

target_include_directories(${target_name}
    INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/..
)
