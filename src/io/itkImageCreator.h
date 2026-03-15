#pragma once

#include <itkImage.h>
#include <QVector>
#include <QByteArray>
#include <cstring>

template<typename T>
itk::DataObject::Pointer createITKImage(
        const QVector<unsigned int>& shape,
        const QVector<double>& spacing,
        const QVector<double>& origin,
        const QVector<QVector<double>>& direction,
        const QByteArray& buffer,
        int numberOfComponents)
{
    constexpr unsigned int Dimension = 3;

    using ImageType = itk::Image<T, Dimension>;
    typename ImageType::Pointer image = ImageType::New();

    // ---------- Region ----------
    typename ImageType::RegionType region;
    typename ImageType::IndexType start;
    typename ImageType::SizeType size;

    start.Fill(0);

    for(unsigned int i=0;i<Dimension;i++)
        size[i] = shape[i];

    region.SetSize(size);
    region.SetIndex(start);

    image->SetRegions(region);

    // ---------- Spacing / Origin ----------
    typename ImageType::SpacingType sp;
    typename ImageType::PointType org;

    for(unsigned int i=0;i<Dimension;i++)
    {
        sp[i] = spacing[i];
        org[i] = origin[i];
    }

    image->SetSpacing(sp);
    image->SetOrigin(org);

    // ---------- Direction ----------
    typename ImageType::DirectionType dir;
    dir.SetIdentity();

    if(direction.size() == Dimension)
    {
        for(unsigned int i=0;i<Dimension;i++)
        {
            for(unsigned int j=0;j<Dimension;j++)
            {
                dir[i][j] = direction[i][j];
            }
        }
    }

    image->SetDirection(dir);

    // ---------- Allocate ----------
    image->Allocate();

    // ---------- Copy Buffer ----------
    std::memcpy(
        image->GetBufferPointer(),
        buffer.constData(),
        buffer.size()
    );

    return image.GetPointer();
}