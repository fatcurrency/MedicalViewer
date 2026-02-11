很好，这个技术栈是**非常标准且工程上可落地的组合**：

* Qt 5.9.8 → UI框架
* ITK 5.2.1 → 医学图像读取 / 处理
* VTK 9.2.0 → 可视化渲染
* CMake → 跨平台构建系统

你现在的目标是：

> 构建一个**基础版医学图像可视化软件（支持NIFTI）**，同时保证后续功能易扩展。

下面我给你一个**工业级可扩展目录结构设计**（偏科研 + 产品化思路），并解释每个模块的职责，保证后续加功能不会崩。

---

# 一、总体设计思想

我们采用：

* **MVC思想**
* **模块化解耦**
* **ITK 负责数据**
* **VTK 负责渲染**
* **Qt 负责UI与交互**

结构分为：

```
App层（UI）
Core层（数据与算法）
IO层（文件读写）
Render层（VTK封装）
Common层（工具类）
```

---

# 二、推荐完整目录结构

```
MedicalViewer/
│
├── CMakeLists.txt
├── cmake/
│   ├── FindITK.cmake
│   ├── FindVTK.cmake
│   └── QtConfig.cmake
│
├── thirdparty/                # 未来可扩展第三方库
│
├── src/
│   │
│   ├── main.cpp
│   │
│   ├── app/                   # UI层（Qt相关）
│   │   ├── MainWindow.h
│   │   ├── MainWindow.cpp
│   │   ├── MainWindow.ui
│   │   ├── ImageViewWidget.h
│   │   ├── ImageViewWidget.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── core/                  # 核心数据层（ITK数据管理）
│   │   ├── ImageData.h
│   │   ├── ImageData.cpp
│   │   ├── ImageManager.h
│   │   ├── ImageManager.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── io/                    # 图像读写模块
│   │   ├── NiftiReader.h
│   │   ├── NiftiReader.cpp
│   │   ├── ImageIOFactory.h
│   │   ├── ImageIOFactory.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── render/                # VTK封装层
│   │   ├── VTKImageViewer.h
│   │   ├── VTKImageViewer.cpp
│   │   ├── VolumeRenderer.h
│   │   ├── VolumeRenderer.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── common/                # 公共工具类
│   │   ├── Logger.h
│   │   ├── Logger.cpp
│   │   ├── Config.h
│   │   ├── Config.cpp
│   │   └── CMakeLists.txt
│   │
│   └── algorithms/            # 后续图像处理扩展（预留）
│       ├── Segmentation/
│       ├── Registration/
│       └── CMakeLists.txt
│
├── resources/
│   ├── icons/
│   ├── qss/
│   └── resources.qrc
│
├── data/                      # 测试数据
│
└── build/
```

---

# 三、各模块职责说明

---

## 1️⃣ app/ —— Qt UI层

只负责：

* 菜单
* 工具栏
* 交互
* 信号槽
* 布局

绝对不要在这里写ITK/VTK逻辑。

例如：

```cpp
void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(...);
    imageManager->loadImage(fileName);
}
```

UI只调用 ImageManager。

---

## 2️⃣ core/ —— 数据核心层

负责：

* 保存ITK图像
* 图像元信息
* 图像状态
* 多图像管理

例如：

```cpp
class ImageData
{
public:
    using ImageType = itk::Image<float, 3>;
    ImageType::Pointer image;
};
```

ImageManager 负责：

* 加载
* 切换当前图像
* 通知渲染层更新

---

## 3️⃣ io/ —— NIFTI读取模块

单独做一个读取模块。

### NiftiReader

```cpp
using ReaderType = itk::ImageFileReader<ImageType>;
reader->SetFileName(filename);
reader->Update();
```

### ImageIOFactory

未来可扩展：

* NIFTI
* DICOM
* MHA
* NRRD

这样以后只需要：

```cpp
auto reader = ImageIOFactory::CreateReader(filename);
```

即可支持多格式。

---

## 4️⃣ render/ —— VTK封装层

这里是重点。

不要在UI中直接写vtk代码。

### VTKImageViewer

负责：

* ITK → VTK 转换
* vtkImageActor
* 三视图显示（未来可扩展）

### VolumeRenderer

负责：

* vtkSmartVolumeMapper
* 体绘制
* 传递函数

ITK转VTK：

```cpp
using ConnectorType =
    itk::ImageToVTKImageFilter<ImageType>;
```

---

## 5️⃣ algorithms/ —— 未来扩展核心

未来你可以添加：

* 心肌分割
* PET-MR融合
* 配准
* AI推理接口

不影响UI结构。

---

# 四、顶层 CMakeLists.txt 示例

```cmake
cmake_minimum_required(VERSION 3.15)
project(MedicalViewer)

set(CMAKE_CXX_STANDARD 17)

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
    Qt5::Widgets
    ${ITK_LIBRARIES}
    ${VTK_LIBRARIES}
)
```

---

# 五、最小可运行功能流程

打开NIFTI → 显示三维

流程：

```
MainWindow
    ↓
ImageManager
    ↓
NiftiReader
    ↓
ImageData
    ↓
VTKImageViewer
    ↓
QVTKOpenGLWidget
```

---

# 六、未来扩展能力

这个架构可以自然扩展：

### ✅ 添加三视图（MPR）

在 render/ 添加：

```
MPRViewer.h
```

---

### ✅ 添加心肌分割

```
algorithms/Segmentation/MyocardiumSegmentation.cpp
```

---

### ✅ 添加PET-MR融合

```
core/MultiModalImageData
render/FusionRenderer
```

---

### ✅ 添加插件系统（高级版本）

可以升级为：

```
plugins/
```

动态加载功能模块。

---

# 七、专业建议（重要）

### 1️⃣ 一定使用 QVTKOpenGLWidget

不要用老的 QVTKWidget。

---

### 2️⃣ 数据层和渲染层一定分离

否则后期改算法会崩。

---

### 3️⃣ 永远不要在UI里写ITK代码

否则你后期做科研会很痛苦。
