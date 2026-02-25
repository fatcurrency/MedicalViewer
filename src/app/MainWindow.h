#pragma once

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
	void showAbout();

private:
	void createMenus();
	void createToolBar();
	void createStatusBar();

	ImageToolWidget* imageToolWidget;
	ImageViewWidget* imageViewWidget;
	
	QAction* openAction;
	QAction* clearAction;
	QAction* exitAction;
	QAction* aboutAction;
};
