#pragma once

#include "io/NiftiReader.h"
#include "render/VTKSliceViewer.h"
#include <QMainWindow>

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
	SliceViewer* sliceViewer;

	NiftiInfoReader niftiInfoReader;
	
	QAction* openAction;
	QAction* clearAction;
	QAction* infoAction;
	QAction* exitAction;
	QAction* aboutAction;
};
