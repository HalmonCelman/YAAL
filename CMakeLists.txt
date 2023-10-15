cmake_minimum_required(VERSION 3.12.0)

add_subdirectory(external/OpenAL)

add_library( YAAL  Sound.h Sound.cpp)

target_link_libraries(YAAL PRIVATE openal)

set_property(TARGET YAAL PROPERTY CXX_STANDARD 20)
