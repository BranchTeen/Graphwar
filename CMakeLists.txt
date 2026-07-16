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

set(_package_dir "${CMAKE_BINARY_DIR}/package_temp/Graphwar")
set(_package_name "Graphwar")

add_custom_target(package_prepare
    COMMAND "${CMAKE_COMMAND}" -E remove_directory "${CMAKE_BINARY_DIR}/package_temp"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${_package_dir}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:Graphwar>"
            "${_package_dir}/"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_BINARY_DIR}/platforms"
            "${_package_dir}/platforms"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_BINARY_DIR}/resources"
            "${_package_dir}/resources"
    COMMENT "Preparing package directory..."
)

if(WIN32)
    add_custom_command(TARGET package_prepare POST_BUILD
        COMMAND cmd /C "if exist \"${CMAKE_BINARY_DIR}\\audio\" ${CMAKE_COMMAND} -E copy_directory \"${CMAKE_BINARY_DIR}\\audio\" \"${_package_dir}\\audio\""
        COMMENT "Copying audio plugins..."
    )
    add_custom_command(TARGET package_prepare POST_BUILD
        COMMAND cmd /C "if exist \"${CMAKE_BINARY_DIR}\\mediaservice\" ${CMAKE_COMMAND} -E copy_directory \"${CMAKE_BINARY_DIR}\\mediaservice\" \"${_package_dir}\\mediaservice\""
        COMMENT "Copying mediaservice plugins..."
    )
    add_custom_command(TARGET package_prepare POST_BUILD
        COMMAND cmd /C "copy /Y \"${CMAKE_BINARY_DIR}\\*.dll\" \"${_package_dir}\\\""
        COMMENT "Copying DLL files..."
    )
elseif(APPLE)
    add_custom_command(TARGET package_prepare POST_BUILD
        COMMAND bash -c "if [ -d \"${CMAKE_BINARY_DIR}/audio\" ]; then ${CMAKE_COMMAND} -E copy_directory \"${CMAKE_BINARY_DIR}/audio\" \"${_package_dir}/audio\"; fi"
        COMMENT "Copying audio plugins..."
    )
    add_custom_command(TARGET package_prepare POST_BUILD
        COMMAND bash -c "if [ -d \"${CMAKE_BINARY_DIR}/mediaservice\" ]; then ${CMAKE_COMMAND} -E copy_directory \"${CMAKE_BINARY_DIR}/mediaservice\" \"${_package_dir}/mediaservice\"; fi"
        COMMENT "Copying mediaservice plugins..."
    )
else()
    add_custom_command(TARGET package_prepare POST_BUILD
        COMMAND bash -c "if [ -d \"${CMAKE_BINARY_DIR}/audio\" ]; then ${CMAKE_COMMAND} -E copy_directory \"${CMAKE_BINARY_DIR}/audio\" \"${_package_dir}/audio\"; fi"
        COMMENT "Copying audio plugins..."
    )
    add_custom_command(TARGET package_prepare POST_BUILD
        COMMAND bash -c "if [ -d \"${CMAKE_BINARY_DIR}/mediaservice\" ]; then ${CMAKE_COMMAND} -E copy_directory \"${CMAKE_BINARY_DIR}/mediaservice\" \"${_package_dir}/mediaservice\"; fi"
        COMMENT "Copying mediaservice plugins..."
    )
    add_custom_command(TARGET package_prepare POST_BUILD
        COMMAND bash -c "cp -f \"${CMAKE_BINARY_DIR}\"/*.so* \"${_package_dir}/\" 2>/dev/null || true"
        COMMENT "Copying shared libraries..."
    )
endif()

if(WIN32)
    add_custom_target(graphwar_package
        COMMAND powershell -Command "Compress-Archive -Path '${CMAKE_BINARY_DIR}/package_temp/*' -DestinationPath '${CMAKE_BINARY_DIR}/${_package_name}.zip' -Force"
        DEPENDS package_prepare
        COMMENT "Generating package ZIP..."
    )
else()
    add_custom_target(graphwar_package
        COMMAND zip -r "${CMAKE_BINARY_DIR}/${_package_name}.zip" "."
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/package_temp"
        DEPENDS package_prepare
        COMMENT "Generating package ZIP..."
    )
endif()

add_dependencies(package_prepare Graphwar)