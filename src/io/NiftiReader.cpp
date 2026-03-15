#include "NiftiReader.h"

#include <algorithm>
#include <limits>

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

    out += "Direction:\n";
    for (unsigned int i = 0; i < dims; ++i)
    {
        std::vector<double> dir = imageIO->GetDirection(i);
        for (unsigned int j = 0; j < dir.size(); ++j)
        {
            out += QString::number(dir[j]);
            if (j + 1 < dir.size())
                out += " ";
        }
        out += "\n";
    }


    out += QString("Component Type: %1\n").arg(QString::fromStdString(imageIO->GetComponentTypeAsString(imageIO->GetComponentType())));
    out += QString("Pixel Type: %1\n").arg(QString::fromStdString(imageIO->GetPixelTypeAsString(imageIO->GetPixelType())));
    out += QString("Number Of Components: %1\n").arg(imageIO->GetNumberOfComponents());

    return out;
}

bool NiftiInfoReader::loadImageInfo(const QString& path)
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
    m_direction.clear();
    m_componentType.clear();
    m_pixelType.clear();
    m_numberOfComponents = 0;

    for(unsigned int i = 0; i < m_dimensions; ++i) {
        m_shape.append(imageIO->GetDimensions(i));
        m_spacing.append(imageIO->GetSpacing(i));
        m_origin.append(imageIO->GetOrigin(i));

        std::vector<double> dir = imageIO->GetDirection(i);
        QVector<double> row;
        for(unsigned int j = 0; j < dir.size(); ++j)
        {
            row.append(dir[j]);
        }

        m_direction.append(row);
    }

    m_componentType = QString::fromStdString(
        imageIO->GetComponentTypeAsString(imageIO->GetComponentType()));

    m_pixelType = QString::fromStdString(
        imageIO->GetPixelTypeAsString(imageIO->GetPixelType()));

    m_numberOfComponents = imageIO->GetNumberOfComponents();

    // ===============================
    // 新增：读取图像数据
    // ===============================
    
    // 总像素数量 = 各维度像素数量的乘积
    size_t totalPixels = 1;
    for(unsigned int i = 0; i < m_dimensions; ++i)
        totalPixels *= imageIO->GetDimensions(i);

    // 每个像素的字节数 = 组件数量 * 每个组件的字节数
    const size_t componentSize = imageIO->GetComponentSize();
    std::cout << "Component Size: " << componentSize << " bytes" << std::endl;

    const size_t totalBytes =
        totalPixels *
        componentSize *
        m_numberOfComponents;

    if (m_numberOfComponents <= 0 || totalBytes == 0)
        return false;

    if (totalBytes > static_cast<size_t>(std::numeric_limits<int>::max()))
        return false;

    m_imageBuffer.resize(static_cast<int>(totalBytes));

    itk::ImageIORegion ioRegion(m_dimensions);
    for (unsigned int i = 0; i < m_dimensions; ++i)
    {
        ioRegion.SetIndex(i, 0);
        ioRegion.SetSize(i, imageIO->GetDimensions(i));
    }
    imageIO->SetIORegion(ioRegion);

    try
    {
        imageIO->Read(m_imageBuffer.data());
    }
    catch(...)
    {
        return false;
    }

    // 转换为ITK和VTK图像对象
    transToITKImage();
    transToVTKImage();

    // ===============================
    // 生成info字符串
    // ===============================

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

    out += "\nDirection:\n";
    for (int i = 0; i < m_direction.size(); ++i)
    {
        for (int j = 0; j < m_direction[i].size(); ++j)
        {
            out += QString::number(m_direction[i][j]);
            if (j + 1 < m_direction[i].size())
                out += " ";
        }
        out += "\n";
    }

    out += QString("\nComponent Type: %1\n").arg(m_componentType);
    out += QString("Pixel Type: %1\n").arg(m_pixelType);
    out += QString("Number Of Components: %1\n").arg(m_numberOfComponents);

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
    m_direction.clear();
    m_componentType.clear();
    m_pixelType.clear();
    m_numberOfComponents = 0;
    return true;
}

bool NiftiInfoReader::clearImageData()
{
    m_imageBuffer.clear();
    m_itkImage = nullptr;
    m_vtkImage = nullptr;
    return true;
}

QByteArray NiftiInfoReader::getImageData() const
{
    return m_imageBuffer;
}

itk::DataObject::Pointer NiftiInfoReader::transToITKImage()
{
    if(m_imageBuffer.isEmpty())
        return nullptr;

    if(m_componentType == "short" || m_componentType == "signed_short")
    {
        m_itkImage = createITKImage<short>(
            m_shape,
            m_spacing,
            m_origin,
            m_direction,
            m_imageBuffer,
            m_numberOfComponents);
    }
    else if(m_componentType == "unsigned_char" || m_componentType == "uchar")
    {
        m_itkImage = createITKImage<unsigned char>(
            m_shape,
            m_spacing,
            m_origin,
            m_direction,
            m_imageBuffer,
            m_numberOfComponents);
    }
    else if(m_componentType == "float")
    {
        m_itkImage = createITKImage<float>(
            m_shape,
            m_spacing,
            m_origin,
            m_direction,
            m_imageBuffer,
            m_numberOfComponents);
    }
    else if(m_componentType == "double")
    {
        m_itkImage = createITKImage<double>(
            m_shape,
            m_spacing,
            m_origin,
            m_direction,
            m_imageBuffer,
            m_numberOfComponents);
    }
    else
    {
        return nullptr;
    }

    m_itkImage->SetObjectName("InitialImage");

    return m_itkImage;
}

vtkImageData* NiftiInfoReader::transToVTKImage()
{
    if(m_imageBuffer.isEmpty())
        return nullptr;

    m_vtkImage = vtkSmartPointer<vtkImageData>::New();

    m_vtkImage->SetDimensions(
        m_shape[0],
        m_shape[1],
        m_shape[2]);

    m_vtkImage->SetSpacing(
        m_spacing[0],
        m_spacing[1],
        m_spacing[2]);

    m_vtkImage->SetOrigin(
        m_origin[0],
        m_origin[1],
        m_origin[2]);

    int vtkType = VTK_SHORT;

    if(m_componentType == "unsigned_char" || m_componentType == "uchar")
        vtkType = VTK_UNSIGNED_CHAR;

    else if(m_componentType == "short" || m_componentType == "signed_short")
        vtkType = VTK_SHORT;

    else if(m_componentType == "unsigned_short" || m_componentType == "ushort")
        vtkType = VTK_UNSIGNED_SHORT;

    else if(m_componentType == "float")
        vtkType = VTK_FLOAT;

    else if(m_componentType == "double")
        vtkType = VTK_DOUBLE;

    m_vtkImage->AllocateScalars(
        vtkType,
        m_numberOfComponents);

    size_t totalBytes = m_imageBuffer.size();

    memcpy(
        m_vtkImage->GetScalarPointer(),
        m_imageBuffer.constData(),
        totalBytes);

    return m_vtkImage;
}

void NiftiInfoReader::printITKImageInfo() const
{
    if (!m_itkImage)
    {
        std::cout << "No ITK image loaded." << std::endl;
        return;
    }

    std::cout << "ITK Image Info:" << std::endl;
    m_itkImage->Print(std::cout);
}

void NiftiInfoReader::printVTKImageInfo() const
{
    if (!m_vtkImage)
    {
        std::cout << "No VTK image loaded." << std::endl;
        return;
    }
    std::cout << "VTK Image Info:" << std::endl;
    m_vtkImage->Print(std::cout);
}