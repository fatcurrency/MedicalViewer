#pragma once

#include <QWidget>
#include <QDebug>
#include <QHBoxLayout>
#include <QPointer>
#include <QVTKOpenGLNativeWidget.h>
#include <QSlider>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkCamera.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>
#include <vtkNamedColors.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

class SliceInteraction;

class SliceViewer : public QWidget
{
    Q_OBJECT
public:
    enum SliceOrientation
    {
        Axial = 0,
        Sagittal,
        Coronal
    };

    explicit SliceViewer(QWidget* parent = nullptr);
    ~SliceViewer() override = default;

    // 设置图像和切片参数
    void setImage(vtkSmartPointer<vtkImageData> image, SliceOrientation orientation);
    // 更新切片显示
    void updateSlice(int sliceIndex);
    // 初始化切片显示（设置默认切片索引和方向）
    void initializeSliceViewer();
    // 获取当前切片索引
    int currentSliceIndex() const { return m_sliceIndex; }
    // 获取当前切片方向
    SliceOrientation currentOrientation() const { return m_orientation; }
    // 设置slider的范围和初始值
    void setupSliceSlider();
    // 清除显示的图像数据
    void clearImage();

private:
    void initializeVTK();
    void setupViewer();

private:
    QHBoxLayout* m_layout;
    QPointer<QVTKOpenGLNativeWidget> m_vtkWidget;
    QSlider* m_sliceSlider;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkImageViewer2> m_imageViewer;

    vtkSmartPointer<vtkImageData> m_vtkImage;
    SliceOrientation m_orientation;
    int m_sliceIndex;
    // 交互处理器，负责处理鼠标滚轮和键盘事件以切换切片
    SliceInteraction* m_interactionHandler;
};