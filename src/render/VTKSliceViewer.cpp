#include "VTKSliceViewer.h"
#include "SliceCameraUtils.h"
#include "SliceInteraction.h"

#include <QSignalBlocker>

SliceViewer::SliceViewer(QWidget* parent)
    : QWidget(parent),
    m_layout(new QHBoxLayout(this)),
    m_vtkWidget(new QVTKOpenGLNativeWidget(this)),
    m_sliceSlider(new QSlider(Qt::Vertical, this)),
    m_renderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
    m_imageViewer(vtkSmartPointer<vtkImageViewer2>::New()),
    m_orientation(Axial),
    m_sliceIndex(0),
    m_interactionHandler(nullptr)
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);
    m_layout->addWidget(m_vtkWidget);
    m_layout->addWidget(m_sliceSlider);
    // 为 m_sliceSlider 添加边框以区分它与渲染区域
    m_sliceSlider->setStyleSheet("QSlider { border: 1px solid gray; }");
    m_sliceSlider->setEnabled(false);
    m_sliceSlider->setMinimum(0);
    m_sliceSlider->setMaximum(0);
    m_sliceSlider->setValue(0);

    setLayout(m_layout);
    initializeVTK();

    // 安装交互处理器：处理鼠标滚轮与键盘切片切换
    m_interactionHandler = new SliceInteraction(this, m_vtkWidget, this);

    // 连接 slider 的值改变信号到切片更新槽
    connect(m_sliceSlider, &QSlider::valueChanged, this, &SliceViewer::updateSlice);
}

void SliceViewer::initializeVTK()
{
    // 先只将渲染窗口挂到 Qt 小部件，图像管线等收到数据后再连接
    m_vtkWidget->SetRenderWindow(m_renderWindow);
    
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    // 平行投影适合医学图像切片显示
    renderer->GetActiveCamera()->ParallelProjectionOn();
    renderer->ResetCamera();
    m_renderWindow->AddRenderer(renderer);

    // 使用图像交互样式
    vtkSmartPointer<vtkInteractorStyleImage> style =
        vtkSmartPointer<vtkInteractorStyleImage>::New();
    if (auto* interactor = m_renderWindow->GetInteractor())
    {
        interactor->SetInteractorStyle(style);
    }
}

void SliceViewer::setImage(vtkSmartPointer<vtkImageData> image, SliceOrientation orientation)
{
    if (!image)
    {
        qWarning() << "Invalid image!";
        return;
    }

    m_vtkImage = image;
    m_orientation = orientation;

    setupViewer();
    initializeSliceViewer();
}

void SliceViewer::setupViewer()
{
    if (!m_vtkImage)
        return;

    m_imageViewer->SetRenderWindow(m_renderWindow);
    m_imageViewer->SetInputData(m_vtkImage);

    int extent[6];
    m_vtkImage->GetExtent(extent);

    switch (m_orientation)
    {
        case Axial:
            m_imageViewer->SetSliceOrientationToXY();
            m_sliceIndex = std::clamp(m_sliceIndex, extent[4], extent[5]);
            break;
        case Coronal:
            m_imageViewer->SetSliceOrientationToXZ();
            m_sliceIndex = std::clamp(m_sliceIndex, extent[2], extent[3]);
            break;
        case Sagittal:
            m_imageViewer->SetSliceOrientationToYZ();
            m_sliceIndex = std::clamp(m_sliceIndex, extent[0], extent[1]);
            break;
    }

    m_imageViewer->SetSlice(m_sliceIndex);
    setupSliceSlider();
    auto* renderer = m_imageViewer->GetRenderer();
    if (renderer){
        renderer->ResetCamera();
        const double preservedParallelScale = renderer->GetActiveCamera()
            ? renderer->GetActiveCamera()->GetParallelScale()
            : 0.0;
        alignCameraToSlice(m_vtkImage, renderer, m_orientation, m_sliceIndex, preservedParallelScale);
    }
    m_imageViewer->Render();
}

void SliceViewer::updateSlice(int sliceIndex)
{
    if (!m_vtkImage)
        return;

    int extent[6];
    m_vtkImage->GetExtent(extent);

    switch (m_orientation)
    {
        case Axial:
            m_sliceIndex = std::clamp(sliceIndex, extent[4], extent[5]);
            break;
        case Coronal:
            m_sliceIndex = std::clamp(sliceIndex, extent[2], extent[3]);
            break;
        case Sagittal:
            m_sliceIndex = std::clamp(sliceIndex, extent[0], extent[1]);
            break;
    }

    auto* renderer = m_imageViewer->GetRenderer();
    const double preservedParallelScale = (renderer && renderer->GetActiveCamera())
        ? renderer->GetActiveCamera()->GetParallelScale()
        : 0.0;

    m_imageViewer->SetSlice(m_sliceIndex);
    if (m_sliceSlider->value() != m_sliceIndex)
    {
        QSignalBlocker blocker(m_sliceSlider);
        m_sliceSlider->setValue(m_sliceIndex);
    }

    // 保持缩放比例不变，同时让相机与当前切片面同轴，避免后半段切片掉出视锥。
    alignCameraToSlice(m_vtkImage, renderer, m_orientation, m_sliceIndex, preservedParallelScale);

    m_imageViewer->Render();
}

void SliceViewer::initializeSliceViewer()
{
    if (!m_vtkImage)
        return;

    int extent[6];
    m_vtkImage->GetExtent(extent);

    // 设置默认切片索引为中间位置
    switch (m_orientation)
    {
        case Axial:
            m_sliceIndex = (extent[4] + extent[5]) / 2;
            break;
        case Coronal:
            m_sliceIndex = (extent[2] + extent[3]) / 2;
            break;
        case Sagittal:
            m_sliceIndex = (extent[0] + extent[1]) / 2;
            break;
    }

    m_imageViewer->SetSlice(m_sliceIndex);
    setupSliceSlider();
    auto* renderer = m_imageViewer->GetRenderer();
    const double preservedParallelScale = (renderer && renderer->GetActiveCamera())
        ? renderer->GetActiveCamera()->GetParallelScale()
        : 0.0;
    alignCameraToSlice(m_vtkImage, renderer, m_orientation, m_sliceIndex, preservedParallelScale);
    m_imageViewer->Render();
}

void SliceViewer::setupSliceSlider()
{
    if (!m_vtkImage){
        m_sliceSlider->setEnabled(false);
        return;
    }else{
        m_sliceSlider->setEnabled(true);
    }


    int extent[6];
    m_vtkImage->GetExtent(extent);

    int minSlice = 0, maxSlice = 0;
    switch (m_orientation)
    {
        case Axial:
            minSlice = extent[4];
            maxSlice = extent[5];
            break;
        case Coronal:
            minSlice = extent[2];
            maxSlice = extent[3];
            break;
        case Sagittal:
            minSlice = extent[0];
            maxSlice = extent[1];
            break;
    }

    m_sliceSlider->setMinimum(minSlice);
    m_sliceSlider->setMaximum(maxSlice);
    m_sliceSlider->setValue(m_sliceIndex);
}

void SliceViewer::clearImage()
{
    m_vtkImage = nullptr;
    m_sliceIndex = 0;
    m_orientation = Axial;

    // 断开 vtkImageViewer2 与渲染窗口的绑定，重置为新实例
    m_imageViewer = vtkSmartPointer<vtkImageViewer2>::New();

    // 移除渲染窗口中的所有 renderer
    auto* rendererCollection = m_renderWindow->GetRenderers();
    rendererCollection->InitTraversal();
    while (auto* ren = rendererCollection->GetNextItem())
    {
        m_renderWindow->RemoveRenderer(ren);
        rendererCollection->InitTraversal(); // 移除后重新遍历
    }

    // 添加一个干净的空 renderer（平行投影）
    auto blankRenderer = vtkSmartPointer<vtkRenderer>::New();
    blankRenderer->SetBackground(0.0, 0.0, 0.0);
    blankRenderer->GetActiveCamera()->ParallelProjectionOn();
    m_renderWindow->AddRenderer(blankRenderer);

    m_sliceSlider->setEnabled(false);
    m_sliceSlider->setMinimum(0);
    m_sliceSlider->setMaximum(0);
    m_sliceSlider->setValue(0);

    m_renderWindow->Render();
}