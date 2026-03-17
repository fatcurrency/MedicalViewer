#pragma once

#include <QObject>

class SliceViewer;
class QWidget;

class SliceInteraction : public QObject
{
    Q_OBJECT
public:
    // 构造函数，接受切片查看器和VTK窗口指针，以及可选的父对象
    explicit SliceInteraction(SliceViewer* viewer, QWidget* vtkWidget, QObject* parent = nullptr);
    ~SliceInteraction() override = default;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    SliceViewer* m_viewer;
    QWidget* m_vtkWidget;
};
