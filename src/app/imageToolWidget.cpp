#include "imageToolWidget.h"

#include <QVBoxLayout>

ImageToolWidget::ImageToolWidget(QWidget* parent)
    : QWidget(parent)
{
    // 目前保持空白，仅做基础占位
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->addStretch();
    setLayout(layout);

    //设置背景颜色为浅蓝色
    setAttribute(Qt::WA_StyledBackground, true); // 确保样式背景被绘制
    setStyleSheet("background-color: #4aeaff;");

}