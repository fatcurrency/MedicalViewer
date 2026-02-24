#pragma once

#include <QMainWindow>

class QAction;
class ImageViewWidget;

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

	ImageViewWidget* imageViewWidget;

	QAction* openAction;
	QAction* clearAction;
	QAction* exitAction;
	QAction* aboutAction;
};
