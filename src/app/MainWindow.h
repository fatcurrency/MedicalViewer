#pragma once

#include "io/NiftiReader.h"
#include <QMainWindow>
#include <QLabel>

class QAction;
class ImageViewWidget;
class ImageToolWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);

private slots:
	void openFile();
	void clearCurrent();
	void showInfo();
	void showAbout();

private:
	void createMenus();
	void createToolBar();
	void createStatusBar();

	ImageToolWidget* imageToolWidget;
	ImageViewWidget* imageViewWidget;
	NiftiInfoReader niftiInfoReader;
	
	QAction* openAction;
	QAction* clearAction;
	QAction* infoAction;
	QAction* exitAction;
	QAction* aboutAction;

	// 状态栏永久标签
	QLabel* m_fileLabel       = nullptr;
	QLabel* m_imageSizeLabel = nullptr;
	void updateStatusBarLabels();
};
