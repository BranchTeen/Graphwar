cmake_minimum_required(VERSION 3.20)
project(Graphwar VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
find_package(FFmpeg REQUIRED COMPONENTS avformat avcodec avutil swresample)

add_subdirectory(src src)
