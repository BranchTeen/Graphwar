cmake_minimum_required(VERSION 3.16)

project(Meitu)

find_package(FLTK CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(code code)
