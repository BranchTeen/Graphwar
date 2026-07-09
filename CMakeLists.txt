cmake_minimum_required(VERSION 3.20)
project(Graphwar VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Linux / macOS 下的 RPATH：让可执行文件从自身所在目录及 lib/ 子目录找共享库
if(NOT WIN32)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/lib")
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/model/Player.h
    src/model/Square.h
    src/model/GameModel.h
    src/model/GamePhase.h
    src/model/GameConfig.h
    src/viewmodel/GameViewModel.h
    src/viewmodel/GameViewModel.cpp
    src/viewmodel/SaveManager.h
    src/viewmodel/SaveManager.cpp
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
    src/viewmodel/parser/Tokenizer.h
    src/viewmodel/parser/Tokenizer.cpp
    src/viewmodel/parser/Expression.h
    src/viewmodel/parser/Expression.cpp
    src/viewmodel/parser/Parser.h
    src/viewmodel/parser/Parser.cpp
    src/viewmodel/parser/Evaluator.h
    src/viewmodel/parser/Evaluator.cpp
    resources/resources.qrc
)

if(WIN32)
    set(SOURCES ${SOURCES} resources/app.rc)
endif()

# Windows：WIN32 表示 GUI 子系统（无控制台窗口）
# macOS：MACOSX_BUNDLE 会生成 .app 包
# Linux：普通可执行文件
if(WIN32)
    add_executable(Graphwar WIN32 ${SOURCES})
elseif(APPLE)
    add_executable(Graphwar MACOSX_BUNDLE ${SOURCES})
else()
    add_executable(Graphwar ${SOURCES})
endif()

target_link_libraries(Graphwar PRIVATE Qt6::Widgets)
target_include_directories(Graphwar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ======= 部署：把 Qt 平台插件（qwindows / libqcocoa / libqxcb）放到 exe 旁 =======
# vcpkg 的 z-applocal 已自动部署所有 Qt DLL 到 build/ 目录（Windows 下），
# 但不会创建 platforms/ 子目录结构 — 这里手动补一下。
if(WIN32)
    set(_qt_plugin "qwindows.dll")
elseif(APPLE)
    set(_qt_plugin "libqcocoa.dylib")
else()
    set(_qt_plugin "libqxcb.so")
endif()

# 1) 如果定义了 vcpkg 变量，按 vcpkg 的 layout 找：<installed>/<triplet>/Qt6/plugins/platforms/...
if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
    set(_candidate "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/Qt6/plugins/platforms/${_qt_plugin}")
    if(EXISTS "${_candidate}")
        set(_qt_platform_plugin "${_candidate}")
    endif()
endif()
# 2) fallback：从 qmake 的位置往上一级后拼接 plugins/platforms/...
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
    message(WARNING "找不到 Qt 平台插件 ${_qt_plugin}；请在运行前把它手工放到可执行文件旁的 platforms/ 目录。")
endif()
