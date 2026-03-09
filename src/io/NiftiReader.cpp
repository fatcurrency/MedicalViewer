#include "NiftiReader.h"

#include <QFileInfo>
#include <QFile>
#include <itkNiftiImageIO.h>
#include <itkImageIOBase.h>

QString NiftiInfoReader::readInfo(const QString& path)
{
    QFileInfo fileInfo(path);
    if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable())
        return {};

    const QString lowerPath = path.toLower();
    if (!lowerPath.endsWith(".nii") && !lowerPath.endsWith(".nii.gz"))
        return {};

    const QByteArray encodedPath = QFile::encodeName(fileInfo.absoluteFilePath());
    const std::string filename(encodedPath.constData(), static_cast<size_t>(encodedPath.size()));

    itk::NiftiImageIO::Pointer imageIO = itk::NiftiImageIO::New();
    if (!imageIO->CanReadFile(filename.c_str()))
        return {};

    imageIO->SetFileName(filename);
    try {
        imageIO->ReadImageInformation();
    } catch (...) {
        return {};
    }

    unsigned int dims = imageIO->GetNumberOfDimensions();

    QString out;
    out += QString("File: %1\n").arg(path);
    out += QString("Dimensions: %1\n").arg(dims);

    // 添加 shape 信息
    out += "Shape: ";
    for (unsigned int i = 0; i < dims; ++i) {
        out += QString::number(imageIO->GetDimensions(i));
        if (i + 1 < dims) out += " x ";
    }
    out += "\n";

    out += "Spacing: ";
    for (unsigned int i = 0; i < dims; ++i) {
        out += QString::number(imageIO->GetSpacing(i));
        if (i + 1 < dims) out += ", ";
    }
    out += "\n";

    out += "Origin: ";
    for (unsigned int i = 0; i < dims; ++i) {
        out += QString::number(imageIO->GetOrigin(i));
        if (i + 1 < dims) out += ", ";
    }
    out += "\n";

    out += QString("Component Type: %1\n").arg(QString::fromStdString(imageIO->GetComponentTypeAsString(imageIO->GetComponentType())));
    out += QString("Pixel Type: %1\n").arg(QString::fromStdString(imageIO->GetPixelTypeAsString(imageIO->GetPixelType())));
    out += QString("Number Of Components: %1\n").arg(imageIO->GetNumberOfComponents());

    return out;
}

bool NiftiInfoReader::loadInfo(const QString& path)
{
    QFileInfo fileInfo(path);
    if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable())
        return false;

    const QString lowerPath = path.toLower();
    if (!lowerPath.endsWith(".nii") && !lowerPath.endsWith(".nii.gz"))
        return false;

    const QByteArray encodedPath = QFile::encodeName(fileInfo.absoluteFilePath());
    const std::string filename(encodedPath.constData(), static_cast<size_t>(encodedPath.size()));

    itk::NiftiImageIO::Pointer imageIO = itk::NiftiImageIO::New();
    if (!imageIO->CanReadFile(filename.c_str()))
        return false;

    imageIO->SetFileName(filename);
    try {
        imageIO->ReadImageInformation();
    } catch (...) {
        return false;
    }

    m_fileName = path;
    m_dimensions = imageIO->GetNumberOfDimensions();
    m_shape.clear();
    m_spacing.clear();
    m_origin.clear();

    for (unsigned int i = 0; i < m_dimensions; ++i) {
        m_shape.append(imageIO->GetDimensions(i));
        m_spacing.append(imageIO->GetSpacing(i));
        m_origin.append(imageIO->GetOrigin(i));
    }

    // 生成info字符串
    QString out;
    out += QString("File: %1\n").arg(m_fileName);
    out += QString("Dimensions: %1\n").arg(m_dimensions);
    out += "Shape: ";
    for (unsigned int i = 0; i < m_shape.size(); ++i) {
        out += QString::number(m_shape[i]);
        if (i + 1 < m_shape.size()) out += " x ";
    }
    out += "\nSpacing: ";
    for (unsigned int i = 0; i < m_spacing.size(); ++i) {
        out += QString::number(m_spacing[i]);
        if (i + 1 < m_spacing.size()) out += ", ";
    }
    out += "\nOrigin: ";
    for (unsigned int i = 0; i < m_origin.size(); ++i) {
        out += QString::number(m_origin[i]);
        if (i + 1 < m_origin.size()) out += ", ";
    }
    out += QString("\nComponent Type: %1\n").arg(QString::fromStdString(imageIO->GetComponentTypeAsString(imageIO->GetComponentType())));
    out += QString("Pixel Type: %1\n").arg(QString::fromStdString(imageIO->GetPixelTypeAsString(imageIO->GetPixelType())));
    out += QString("Number Of Components: %1\n").arg(imageIO->GetNumberOfComponents());

    m_info = out;
    return true;
}

QString NiftiInfoReader::getInfo() const
{
    return m_info;
}

bool NiftiInfoReader::clearInfo()
{
    m_info.clear();
    m_fileName.clear();
    m_dimensions = 0;
    m_shape.clear();
    m_spacing.clear();
    m_origin.clear();
    return true;
}