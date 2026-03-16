#include "VTKSliceViewer.h"

SliceViewer::SliceViewer(QWidget* parent)
    : QWidget(parent),
    m_layout(new QHBoxLayout(this)),
    m_vtkWidget(new QVTKOpenGLNativeWidget(this)),
    m_sliceSlider(new QSlider(Qt::Vertical, this)),
    m_renderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
    m_imageViewer(vtkSmartPointer<vtkImageViewer2>::New()),
    m_orientation(Axial),
    m_sliceIndex(0)
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);
    m_layout->addWidget(m_vtkWidget);
    m_layout->addWidget(m_sliceSlider);

    setLayout(m_layout);
    initializeVTK();

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
    // 给 VTK 图像查看器启用【平行投影】
    auto* renderer = m_imageViewer->GetRenderer();
    auto* camera = renderer ? renderer->GetActiveCamera() : nullptr;
    if (camera){
        camera->ParallelProjectionOn();
    }
    m_imageViewer->GetRenderer()->ResetCamera();
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

    m_imageViewer->SetSlice(m_sliceIndex);
    m_sliceSlider->setValue(m_sliceIndex);

    // 保存当前视图的缩放比例 → 强制开启平行投影（不变形）→ 恢复之前的缩放 → 重置裁剪范围 → 刷新显示。
    auto* renderer = m_imageViewer->GetRenderer();
    auto* camera = renderer ? renderer->GetActiveCamera() : nullptr;
    const double preservedParallelScale = camera ? camera->GetParallelScale() : 0.0;
    if (camera){
        camera->ParallelProjectionOn();
        if (preservedParallelScale > 0.0){
            camera->SetParallelScale(preservedParallelScale);
        }
    }
    if (renderer){
        renderer->ResetCameraClippingRange();
    }
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
    // 给 VTK 图像查看器启用【平行投影】
    auto* renderer = m_imageViewer->GetRenderer();
    auto* camera = renderer ? renderer->GetActiveCamera() : nullptr;
    if (camera){
        camera->ParallelProjectionOn();
    }
    if (renderer){
        renderer->ResetCameraClippingRange();
    }
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