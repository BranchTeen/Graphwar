
add_executable(AirBattle main.cpp
	common/frame.cpp
	viewmodel/commands/NextStepCommand.cpp
	viewmodel/commands/MoveRedCommand.cpp
	viewmodel/GameViewModel.cpp
	viewmodel/SpiritViewModel.cpp
	view/widgets/BattleBoard.cpp
	view/MainWindow.cpp
	app/AirApp.cpp)

target_precompile_headers(AirBattle PRIVATE precomp.h)

set_target_properties(AirBattle PROPERTIES
	WIN32_EXECUTABLE TRUE)

target_link_libraries(AirBattle PRIVATE fltk fltk_images)

set(assets_path_list "${CMAKE_CURRENT_BINARY_DIR}/Debug/assets"
	"${CMAKE_CURRENT_BINARY_DIR}/Release/assets")
file(MAKE_DIRECTORY ${assets_path_list})
foreach(assets_path ${assets_path_list})
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/bullet.png"
		"${CMAKE_CURRENT_SOURCE_DIR}/assets/plane_red.png"
		"${CMAKE_CURRENT_SOURCE_DIR}/assets/plane_blue.png"
		DESTINATION "${assets_path}")
endforeach()
