add_library(MedicalViewerApp
	MainWindow.h
	MainWindow.cpp
	ImageViewWidget.h
	ImageViewWidget.cpp
)

target_link_libraries(MedicalViewerApp
	PUBLIC
	Qt5::Widgets
)

target_include_directories(MedicalViewerApp
	PUBLIC
	${CMAKE_SOURCE_DIR}/src
)
