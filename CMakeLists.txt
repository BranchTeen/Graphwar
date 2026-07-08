cmake_minimum_required(VERSION 3.20)
project(Graphwar VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/model/Player.h
    src/model/Square.h
    src/model/GameModel.h
    src/model/GamePhase.h
    src/parser/Token.h
    src/parser/Tokenizer.h
    src/parser/Tokenizer.cpp
    src/parser/Expression.h
    src/parser/Expression.cpp
    src/parser/Parser.h
    src/parser/Parser.cpp
    src/parser/Evaluator.h
    src/parser/Evaluator.cpp
    src/viewmodel/GameViewModel.h
    src/viewmodel/GameViewModel.cpp
    src/view/MainWindow.h
    src/view/MainWindow.cpp
    src/view/GameCanvas.h
    src/view/GameCanvas.cpp
    src/view/FunctionInput.h
    src/view/FunctionInput.cpp
    src/utils/Geometry.h
)

add_executable(Graphwar ${SOURCES})

target_link_libraries(Graphwar PRIVATE Qt6::Widgets)
target_include_directories(Graphwar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
