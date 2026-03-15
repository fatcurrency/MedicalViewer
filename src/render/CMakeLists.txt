# src/render/CMakeLists.txt

add_library(MedicalViewerRender
    VTKSliceViewer.cpp
    VTKSliceViewer.h
)

# 链接依赖库
target_link_libraries(MedicalViewerRender 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ${VTK_LIBRARIES}
    )

# 包含头文件目录（可根据实际情况修改）
target_include_directories(MedicalViewerRender 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

