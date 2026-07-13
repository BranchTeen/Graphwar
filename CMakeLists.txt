cmake_minimum_required(VERSION 3.20)
project(Graphwar VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT WIN32)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/lib")
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/app/GraphwarApp.h
    src/app/GraphwarApp.cpp
    src/common/EventBus.h
    src/common/EventForwarder.h
    src/common/property_ids.h
    src/common/frame.h
    src/common/frame.cpp
    src/model/Player.h
    src/model/Square.h
    src/model/GameModel.h
    src/model/GameModel.cpp
    src/model/GamePhase.h
    src/model/GameConfig.h
    src/model/SaveInfo.h
    src/model/SaveInfo.cpp
    src/model/SaveManager.h
    src/model/SaveManager.cpp
    src/model/parser/Token.h
    src/model/parser/Tokenizer.h
    src/model/parser/Tokenizer.cpp
    src/model/parser/Expression.h
    src/model/parser/Expression.cpp
    src/model/parser/Parser.h
    src/model/parser/Parser.cpp
    src/model/parser/Evaluator.h
    src/model/parser/Evaluator.cpp
    src/model/parser/ShuntingYard.h
    src/model/parser/ShuntingYard.cpp
    src/viewmodel/GameViewModel.h
    src/viewmodel/GameViewModel.cpp
    src/view/MainWindow.h
    src/view/MainWindow.cpp
    src/view/GameCanvas.h
    src/view/GameCanvas.cpp
    src/view/FunctionInput.h
    src/view/FunctionInput.cpp
    src/view/SaveManagerPage.h
    src/view/SaveManagerPage.cpp
    src/view/PauseMenuPage.h
    src/view/PauseMenuPage.cpp
    src/view/ConfigPage.h
    src/view/ConfigPage.cpp
    src/utils/Geometry.h
    resources/resources.qrc
)

if(WIN32)
    set(SOURCES ${SOURCES} resources/app.rc)
endif()

if(WIN32)
    add_executable(Graphwar WIN32 ${SOURCES})
elseif(APPLE)
    add_executable(Graphwar MACOSX_BUNDLE ${SOURCES})
else()
    add_executable(Graphwar ${SOURCES})
endif()

target_link_libraries(Graphwar PRIVATE Qt6::Widgets)
target_include_directories(Graphwar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ======= Deploy: Qt platform plugin =======
if(WIN32)
    set(_qt_plugin "qwindows.dll")
elseif(APPLE)
    set(_qt_plugin "libqcocoa.dylib")
else()
    set(_qt_plugin "libqxcb.so")
endif()

if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
    set(_candidate "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/Qt6/plugins/platforms/${_qt_plugin}")
    if(EXISTS "${_candidate}")
        set(_qt_platform_plugin "${_candidate}")
    endif()
endif()

if(NOT DEFINED _qt_platform_plugin)
    get_target_property(_qmake_loc Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_loc}" DIRECTORY)
    get_filename_component(_qt_prefix_dir "${_qt_bin_dir}" DIRECTORY)
    set(_candidate "${_qt_prefix_dir}/plugins/platforms/${_qt_plugin}")
    if(EXISTS "${_candidate}")
        set(_qt_platform_plugin "${_candidate}")
    endif()
endif()

if(DEFINED _qt_platform_plugin AND EXISTS "${_qt_platform_plugin}")
    add_custom_command(TARGET Graphwar POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory
                "$<TARGET_FILE_DIR:Graphwar>/platforms"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${_qt_platform_plugin}"
                "$<TARGET_FILE_DIR:Graphwar>/platforms/${_qt_plugin}"
        COMMENT "Deploying Qt platform plugin: ${_qt_platform_plugin}"
    )
else()
    message(WARNING "Qt platform plugin ${_qt_plugin} not found; please manually copy to platforms/ directory.")
endif()
