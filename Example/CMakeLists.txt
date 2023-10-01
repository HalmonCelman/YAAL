cmake_minimum_required(VERSION 3.12.0)

project(Example)

link_libraries(-lopenal)

add_library(YAAL ../Sound.cpp ../Sound.h)

add_executable(Example main.cpp)

set_property(TARGET YAAL PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} YAAL)

target_include_directories(${PROJECT_NAME} PRIVATE ../)