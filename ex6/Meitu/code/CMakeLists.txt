
add_executable(Pretty main.cpp
	common/frame.cpp
	model/base/image_convert.cpp
	model/ImageModel.cpp
	viewmodel/ImageViewModel.cpp
	view/MainWindow.cpp
	app/PrettyApp.cpp)

target_precompile_headers(Pretty PRIVATE precomp.h)

target_link_libraries(Pretty PRIVATE fltk fltk_images)
