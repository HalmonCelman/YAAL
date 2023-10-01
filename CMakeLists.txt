cmake_minimum_required(VERSION 3.12.0)

link_libraries(-lopenal)

add_library( YAAL  Sound.h Sound.cpp)

set_property(TARGET YAAL PROPERTY CXX_STANDARD 20)
