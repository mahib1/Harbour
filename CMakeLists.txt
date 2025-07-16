cmake_minimum_required(VERSION 3.16)
project(Harbour VERSION 1.0.0 LANGUAGES C CXX) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB IMPL_SOURCES "src/impl/*.cpp")

add_library(harbour_lib STATIC ${IMPL_SOURCES})

target_include_directories(harbour_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/json/include
)

option(BUILD_TESTS "Enable test-only build" OFF)

if(BUILD_TESTS)
    message(STATUS "Test-only mode enabled. Building each test as a separate executable.")

    # Find all test source files
    file(GLOB TEST_SOURCES "tests/*.cpp")

    foreach(test_source ${TEST_SOURCES})
        get_filename_component(target_name ${test_source} NAME_WE)

        add_executable(${target_name} ${test_source})

        target_link_libraries(${target_name} PRIVATE harbour_lib)
    endforeach()

else()
    message(STATUS "Main application mode enabled. Building 'Harbour' executable.")

    add_executable(Harbour src/main.cpp)
    
    target_link_libraries(Harbour PRIVATE harbour_lib)

endif()
