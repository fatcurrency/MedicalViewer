#pragma once

#include "itkImageCreator.h"

#include <QString>
#include <QVector>
#include <QFileInfo>
#include <QFile>
#include <QByteArray>

#include <itkNiftiImageIO.h>
#include <itkImageIOBase.h>
#include <itkDataObject.h>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkType.h>


class NiftiInfoReader
{
public:
    // static 让这个函数可以直接通过类名调用，而不是通过对象调用。
    static QString readInfo(const QString& path);

    bool loadImageInfo(const QString& path);
    QString getInfo() const;

    bool reset();
    bool clearInfo();
    bool clearImageData();

    // 获取图像数据（Raw）
    QByteArray getImageData() const;
    // 获取 ITK 图像对象
    itk::DataObject::Pointer transToITKImage();
    // 获取 VTK 图像对象
    vtkImageData* transToVTKImage();

    // 新增getter
    QString fileName() const { return m_fileName; }
    unsigned int dimensions() const { return m_dimensions; }
    QVector<unsigned int> shape() const { return m_shape; }
    QVector<double> spacing() const { return m_spacing; }
    QVector<double> origin() const { return m_origin; }

    QString componentType() const { return m_componentType; }
    QString pixelType() const { return m_pixelType; }

    int numberOfComponents() const { return m_numberOfComponents; }

    itk::DataObject::Pointer itkImage() const { return m_itkImage; }
    vtkSmartPointer<vtkImageData> vtkImage() const { return m_vtkImage; }

    //  打印 m_itkImage 的基本信息（维度、像素类型等）
    void printITKImageInfo() const;
    // 打印 m_vtkImage 的基本信息（维度、像素类型等）
    void printVTKImageInfo() const;

private:
    QByteArray m_imageBuffer;
    itk::DataObject::Pointer m_itkImage;
    vtkSmartPointer<vtkImageData> m_vtkImage;

    QString m_info;
    QString m_fileName;

    unsigned int m_dimensions = 0;

    QVector<unsigned int> m_shape;
    QVector<double> m_spacing;
    QVector<double> m_origin;
    QVector<QVector<double>> m_direction;
    QString m_componentType;
    QString m_pixelType;

    int m_numberOfComponents = 0;
};