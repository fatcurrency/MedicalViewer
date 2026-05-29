cmake_minimum_required(VERSION 3.15)
project(MedicalViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# VTK (9.x module system: no VTK_USE_FILE)
find_package(VTK 9 REQUIRED COMPONENTS
    CommonCore
    CommonColor
    CommonDataModel
    CommonMath
    ImagingCore
    ImagingColor
    InteractionImage
    InteractionStyle
    RenderingCore
    RenderingOpenGL2
    RenderingFreeType
    GUISupportQt
)

add_subdirectory(src/app)
add_subdirectory(src/core)
add_subdirectory(src/io)
add_subdirectory(src/render)
add_subdirectory(src/common)
add_subdirectory(src/algorithms)

add_executable(MedicalViewer src/main.cpp)

target_link_libraries(MedicalViewer
    MedicalViewerApp
    Qt5::Widgets
    ${ITK_LIBRARIES}
    ${VTK_LIBRARIES}
)

# Initialize VTK object factories (rendering/interaction backends)
vtk_module_autoinit(
    TARGETS MedicalViewer MedicalViewerApp MedicalViewerRender MedicalViewerIO
    MODULES ${VTK_LIBRARIES}
)
