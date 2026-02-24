#pragma once

#include <QWidget>

class QLabel;

class ImageViewWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ImageViewWidget(QWidget* parent = nullptr);

public slots:
	void setFilePath(const QString& filePath);
	void clearView();

private:
	QLabel* titleLabel;
	QLabel* filePathLabel;
};
