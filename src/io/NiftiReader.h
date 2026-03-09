#pragma once

#include <QString>
#include <QVector>

class NiftiInfoReader
{
public:
    // static 让这个函数可以直接通过类名调用，而不是通过对象调用。
    static QString readInfo(const QString& path);
    bool loadInfo(const QString& path);
    // const 关键字用于限定变量或函数的只读性，表示该函数不会修改类的成员变量。
    QString getInfo() const;
    bool clearInfo();

    // 新增getter
    QString fileName() const { return m_fileName; }
    unsigned int dimensions() const { return m_dimensions; }
    QVector<unsigned int> shape() const { return m_shape; }
    QVector<double> spacing() const { return m_spacing; }
    QVector<double> origin() const { return m_origin; }

private:
    QString m_info;
    QString m_fileName;
    unsigned int m_dimensions = 0;
    QVector<unsigned int> m_shape;
    QVector<double> m_spacing;
    QVector<double> m_origin;
};