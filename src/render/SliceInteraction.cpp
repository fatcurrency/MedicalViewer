#include "SliceInteraction.h"
#include "VTKSliceViewer.h"

#include <QWheelEvent>
#include <QKeyEvent>
#include <QEvent>
#include <cstdlib>

SliceInteraction::SliceInteraction(SliceViewer* viewer, QWidget* vtkWidget, QObject* parent)
    : QObject(parent), m_viewer(viewer), m_vtkWidget(vtkWidget)
{
    if (m_vtkWidget)
        // 给VTK窗口安装事件过滤器
        m_vtkWidget->installEventFilter(this); // 让当前类接管这两个控件的所有事件,以便处理鼠标滚轮和键盘事件
    if (m_viewer)
        // 给切片查看器安装事件过滤器（处理键盘事件）
        m_viewer->installEventFilter(this); // 让当前类接管这两个控件的所有事件,以便处理鼠标滚轮和键盘事件
}

// 拦截所有发给 m_vtkWidget 和 m_viewer 的事件
bool SliceInteraction::eventFilter(QObject* watched, QEvent* event)
{
    if (!m_viewer)
        // 如果没有关联的查看器，直接调用基类事件过滤器
        return QObject::eventFilter(watched, event);

    if (event->type() == QEvent::Wheel) {
        auto* we = static_cast<QWheelEvent*>(event); // 将事件转换为 QWheelEvent 以访问滚轮信息
        int ang = we->angleDelta().y(); // 获取垂直滚轮的角度增量，正数表示向上滚动，负数表示向下滚动
        if (ang == 0) // 有些平台可能使用 pixelDelta 而不是 angleDelta
            ang = we->delta();

        int steps = ang / 120; // 每120度 = 1格滚轮
        if (steps != 0) {
            // 计算新的切片索引，向上滚动（正数）应该减少索引，向下滚动（负数）应该增加索引
            const int target = m_viewer->currentSliceIndex() - steps;
            m_viewer->updateSlice(target);
            return true;
        }
    }
    else if (event->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(event); // 将事件转换为 QKeyEvent 以访问键盘信息
        const int key = ke->key(); // 获取按下的键值，Qt::Key_Up、Qt::Key_Down、Qt::Key_PageUp 和 Qt::Key_PageDown 分别对应向上滚动和向下滚动的键
        if (key == Qt::Key_Up || key == Qt::Key_PageUp) {
            m_viewer->updateSlice(m_viewer->currentSliceIndex() - 1);
            return true;
        }
        else if (key == Qt::Key_Down || key == Qt::Key_PageDown) {
            m_viewer->updateSlice(m_viewer->currentSliceIndex() + 1);
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}
