set(SOURCES
    main.cpp
    app/GraphwarApp.cpp
    common/frame.cpp
    model/GameModel.cpp
    common/SaveInfo.cpp
    model/SaveManager.cpp
    model/parser/Tokenizer.cpp
    model/parser/Expression.cpp
    model/parser/Parser.cpp
    model/parser/Evaluator.cpp
    model/parser/ShuntingYard.cpp
    viewmodel/GameViewModel.cpp
    viewmodel/commands/NewGameCommand.cpp
    viewmodel/commands/LaunchCommand.cpp
    viewmodel/commands/UpdateCostPreviewCommand.cpp
    viewmodel/commands/NextTurnCommand.cpp
    viewmodel/commands/PauseCommand.cpp
    viewmodel/commands/ResumeCommand.cpp
    viewmodel/commands/SetConfigCommand.cpp
    viewmodel/commands/SaveSlotCommand.cpp
    viewmodel/commands/LoadSlotCommand.cpp
    viewmodel/commands/DeleteSlotCommand.cpp
    view/MainWindow.cpp
    view/widgets/GameCanvas.cpp
    view/widgets/FunctionInput.cpp
    view/widgets/ConfigPage.cpp
    view/widgets/SaveManagerPage.cpp
    view/widgets/PauseMenuPage.cpp
    ../resources/resources.qrc
)

if(WIN32)
    set(SOURCES ${SOURCES} ../resources/app.rc)
endif()

add_executable(Graphwar ${SOURCES})

set_target_properties(Graphwar PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_precompile_headers(Graphwar PRIVATE precomp.h)

target_include_directories(Graphwar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Graphwar PRIVATE Qt6::Widgets)

if(WIN32)
    set_target_properties(Graphwar PROPERTIES WIN32_EXECUTABLE TRUE)
    set(_qt_plugin "qwindows.dll")
elseif(APPLE)
    set_target_properties(Graphwar PROPERTIES MACOSX_BUNDLE TRUE)
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
