# src/io/CMakeLists.txt

add_library(MedicalViewerIO
    NiftiReader.cpp
    NiftiReader.h
    itkImageCreator.h
)

# 链接依赖库
target_link_libraries(MedicalViewerIO 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ${ITK_LIBRARIES}
    ${VTK_LIBRARIES}
    )

# 包含头文件目录（可根据实际情况修改）
target_include_directories(MedicalViewerIO 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

