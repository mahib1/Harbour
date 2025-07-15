cmake_minimum_required(VERSION 3.16)
project(Harbour VERSION 1.0.0 LANGUAGES C CXX) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
  include
  external/json/include
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Sources
file(GLOB SOURCES src/*.cpp)

add_executable(Harbour ${SOURCES})

# target_link_libraries()
# target_include_directories()
