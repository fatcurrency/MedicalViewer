#pragma once

#include "render/VTKSliceViewer.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

class QLabel;

class ImageViewWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ImageViewWidget(QWidget* parent = nullptr);
	void setVtkImageData(vtkSmartPointer<vtkImageData> image);
	void updateTextLabel(const QString& text);

public slots:
	void clearView();

private:
	QGridLayout* twotwoGridLayout;
	QLabel* textLabel;
	SliceViewer* axialSliceViewer;
	SliceViewer* sagittalSliceViewer;
	SliceViewer* coronalSliceViewer;
};
