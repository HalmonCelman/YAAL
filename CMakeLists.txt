cmake_minimum_required(VERSION 3.12)

link_libraries(-lopenal)

project(OPENAL)

add_library( Sound  Sound.h Sound.cpp)

set_property(TARGET Sound PROPERTY CXX_STANDARD 20)

add_executable( OPENAL main.cpp )

set_property(TARGET OPENAL PROPERTY CXX_STANDARD 20)

target_link_libraries( OPENAL Sound ) 