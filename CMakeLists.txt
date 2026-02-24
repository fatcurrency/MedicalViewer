cmake_minimum_required(VERSION 3.15)
project(MedicalViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

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
