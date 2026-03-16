#include "ImageViewWidget.h"

ImageViewWidget::ImageViewWidget(QWidget* parent)
	: QWidget(parent),
	  twotwoGridLayout(new QGridLayout()),
	  textLabel(nullptr),
	  axialSliceViewer(new SliceViewer(this)),
	  sagittalSliceViewer(new SliceViewer(this)),
	  coronalSliceViewer(new SliceViewer(this))
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(5, 5, 5, 5);
	layout->setSpacing(3);

	// 2x2 网格布局
	twotwoGridLayout->addWidget(axialSliceViewer, 0, 0);
	twotwoGridLayout->addWidget(sagittalSliceViewer, 0, 1);
	twotwoGridLayout->addWidget(coronalSliceViewer, 1, 0);
	twotwoGridLayout->setRowStretch(0, 1);
	twotwoGridLayout->setRowStretch(1, 1);
	twotwoGridLayout->setColumnStretch(0, 1);
	twotwoGridLayout->setColumnStretch(1, 1);
	twotwoGridLayout->setSpacing(3);
	twotwoGridLayout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(twotwoGridLayout);

	textLabel = new QLabel(tr("Medical image display area"), this);
	auto titleFont = textLabel->font();
	titleFont.setPointSize(10);
	titleFont.setBold(true);
	textLabel->setFont(titleFont);
	// 限制文本标签的最大高度，避免占用过多空间
	textLabel->setMaximumHeight(15);

	// layout->addStretch();
	layout->addWidget(textLabel);
	

	setLayout(layout);
}

void ImageViewWidget::setVtkImageData(vtkSmartPointer<vtkImageData> image)
{
	if (!image)
	{
		qWarning() << "Invalid image data!";
		return;
	}

	axialSliceViewer->setImage(image, SliceViewer::Axial);
	sagittalSliceViewer->setImage(image, SliceViewer::Sagittal);
	coronalSliceViewer->setImage(image, SliceViewer::Coronal);

	textLabel->setText(tr("Displaying axial, sagittal, and coronal views"));
}

void ImageViewWidget::clearView()
{
	textLabel->setText(tr("No images currently loaded"));
	axialSliceViewer->clearImage();
	sagittalSliceViewer->clearImage();
	coronalSliceViewer->clearImage();
}

void ImageViewWidget::updateTextLabel(const QString& text)
{
	textLabel->setText(text);
}
