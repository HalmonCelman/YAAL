cmake_minimum_required(VERSION 3.12.0)

link_libraries(-lopenal)

add_library( Sound  Sound.h Sound.cpp)

set_property(TARGET Sound PROPERTY CXX_STANDARD 20)
