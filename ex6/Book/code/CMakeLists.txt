
add_executable(BookHouse main.cpp
	common/frame.cpp
	model/InfoModel.cpp
	viewmodel/commands/ChangePriceCommand.cpp
	viewmodel/commands/LoadCommand.cpp
	viewmodel/BookViewModel.cpp
	view/MainWindow.cpp
	app/BookApp.cpp)

target_precompile_headers(BookHouse PRIVATE precomp.h)

target_link_libraries(BookHouse PRIVATE fltk)
