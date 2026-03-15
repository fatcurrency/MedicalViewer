#include "VTKSliceViewer.h"

SliceViewer::SliceViewer(QWidget* parent)
    : QWidget(parent),
      m_layout(new QVBoxLayout(this)),
    m_vtkWidget(new QVTKOpenGLNativeWidget(this)),
      m_renderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
      m_imageViewer(vtkSmartPointer<vtkImageViewer2>::New()),
      m_orientation(Axial),
      m_sliceIndex(0)
{
    setLayout(m_layout);
    initializeVTK();
}

void SliceViewer::initializeVTK()
{
    // 先只将渲染窗口挂到 Qt 小部件，图像管线等收到数据后再连接
    m_vtkWidget->SetRenderWindow(m_renderWindow);
    m_layout->addWidget(m_vtkWidget);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
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
    m_imageViewer->Render();
}