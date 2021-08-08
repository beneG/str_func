cmake_minimum_required(VERSION 3.9)

project(shared VERSION 1.0.1 DESCRIPTION "shared library test project")

add_library(shared SHARED shared.cpp)

set_target_properties(shared PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(shared PROPERTIES SOVERSION 1)

set_target_properties(shared PROPERTIES PUBLIC_HEADER shared.h)


target_include_directories(shared PRIVATE .)

# or
#target_include_directories(shared PRIVATE include)
#target_include_directories(shared PRIVATE src)


add_executable(main main.cpp)
set_target_properties(main PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(main dl)

