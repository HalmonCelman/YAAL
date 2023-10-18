cmake_minimum_required(VERSION 3.12.0)

project(Example)

add_subdirectory(YAAL)

add_executable(${PROJECT_NAME} main.cpp)

set_property(TARGET YAAL PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} YAAL)
target_include_directories(${PROJECT_NAME} PRIVATE YAAL)