set(SOURCES
    main.cpp
    app/GraphwarApp.cpp
    common/frame.cpp
    model/AudioManager.cpp
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
    viewmodel/commands/SetBgmVolumeCommand.cpp
    viewmodel/commands/SetSfxVolumeCommand.cpp
    viewmodel/commands/SetBgmMutedCommand.cpp
    viewmodel/commands/SetSfxMutedCommand.cpp
    view/MainWindow.cpp
    view/widgets/GameCanvas.cpp
    view/widgets/FunctionInput.cpp
    view/widgets/ConfigPage.cpp
    view/widgets/SaveManagerPage.cpp
    view/widgets/PauseMenuPage.cpp
    view/widgets/GuidePage.cpp
    ../resources/resources.qrc
)

if(WIN32)
    set(SOURCES ${SOURCES} ../resources/app.rc)
endif()

add_executable(Graphwar ${SOURCES})

set_target_properties(Graphwar PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_precompile_headers(Graphwar PRIVATE precomp.h)

target_include_directories(Graphwar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Graphwar PRIVATE Qt6::Widgets Qt6::Multimedia)

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

add_custom_command(TARGET Graphwar POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory
            "$<TARGET_FILE_DIR:Graphwar>/resources"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_SOURCE_DIR}/resources/AIZO-8bit.m4a"
            "$<TARGET_FILE_DIR:Graphwar>/resources/AIZO-8bit.m4a"
    COMMENT "Copying BGM resource"
)

if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
    set(_qt_multimedia_plugins "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/Qt6/plugins/audio")
    if(EXISTS "${_qt_multimedia_plugins}")
        file(GLOB _audio_plugins "${_qt_multimedia_plugins}/*")
        if(_audio_plugins)
            add_custom_command(TARGET Graphwar POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E make_directory
                        "$<TARGET_FILE_DIR:Graphwar>/audio"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        ${_audio_plugins}
                        "$<TARGET_FILE_DIR:Graphwar>/audio/"
                COMMENT "Deploying Qt audio plugins"
            )
        endif()
    endif()

    set(_qt_multimedia_plugins_media "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/Qt6/plugins/mediaservice")
    if(EXISTS "${_qt_multimedia_plugins_media}")
        file(GLOB _media_plugins "${_qt_multimedia_plugins_media}/*")
        if(_media_plugins)
            add_custom_command(TARGET Graphwar POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E make_directory
                        "$<TARGET_FILE_DIR:Graphwar>/mediaservice"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        ${_media_plugins}
                        "$<TARGET_FILE_DIR:Graphwar>/mediaservice/"
                COMMENT "Deploying Qt mediaservice plugins"
            )
        endif()
    endif()

    file(GLOB _ffmpeg_dlls
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/avcodec*.dll"
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/avformat*.dll"
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/avutil*.dll"
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/swresample*.dll"
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/swscale*.dll"
    )
    if(_ffmpeg_dlls)
        add_custom_command(TARGET Graphwar POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    ${_ffmpeg_dlls}
                    "$<TARGET_FILE_DIR:Graphwar>/"
            COMMENT "Copying FFmpeg DLLs for Qt Multimedia FFmpeg backend"
        )
    endif()
endif()
