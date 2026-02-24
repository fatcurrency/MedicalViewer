#include "ImageViewWidget.h"

#include <QLabel>
#include <QVBoxLayout>

ImageViewWidget::ImageViewWidget(QWidget* parent)
	: QWidget(parent)
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(20, 20, 20, 20);
	layout->setSpacing(12);

	titleLabel = new QLabel(tr("Medical image display area (basic version)"), this);
	auto titleFont = titleLabel->font();
	titleFont.setPointSize(14);
	titleFont.setBold(true);
	titleLabel->setFont(titleFont);

	filePathLabel = new QLabel(tr("No images currently loaded"), this);
	filePathLabel->setWordWrap(true);

	layout->addWidget(titleLabel);
	layout->addWidget(filePathLabel);
	layout->addStretch();

	setLayout(layout);
}

void ImageViewWidget::setFilePath(const QString& filePath)
{
	if (filePath.isEmpty()) {
		clearView();
		return;
	}

	filePathLabel->setText(tr("Selected file:\n%1").arg(filePath));
}

void ImageViewWidget::clearView()
{
	filePathLabel->setText(tr("No images currently loaded"));
}
