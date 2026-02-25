#include "MainWindow.h"

#include "ImageViewWidget.h"
#include "imageToolWidget.h"

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QWidget>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , imageToolWidget(new ImageToolWidget(this))
    , imageViewWidget(new ImageViewWidget(this))
    , openAction(nullptr)
    , clearAction(nullptr)
    , exitAction(nullptr)
    , aboutAction(nullptr)
{
    setWindowTitle(tr("MedicalViewer"));
    resize(1200, 800);

    auto* central = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(imageToolWidget, 1);
    mainLayout->addWidget(imageViewWidget, 5);

    central->setLayout(mainLayout);
    setCentralWidget(central);

    createMenus();
    createToolBar();
    createStatusBar();
}

void MainWindow::openFile()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Medical Image"),
        QString(),
        tr("Medical Images (*.nii *.nii.gz);;All Files (*.*)"));

    if (fileName.isEmpty()) {
        statusBar()->showMessage(tr("No file selected"), 2000);
        return;
    }

    imageViewWidget->setFilePath(fileName);
    statusBar()->showMessage(tr("Loaded: %1").arg(fileName), 3000);
}

void MainWindow::clearCurrent()
{
    imageViewWidget->clearView();
    statusBar()->showMessage(tr("Current display cleared"), 2000);
}

void MainWindow::showAbout()
{
    QMessageBox::about(
        this,
        tr("About MedicalViewer"),
        tr("MedicalViewer Basic Version\n"
           "- Qt UI: Basic window, menu and toolbar\n"
           "- Subsequent versions will integrate ITK/VTK display pipeline"));
}

void MainWindow::createMenus()
{
    auto* fileMenu = menuBar()->addMenu(tr("File(&F)"));
    openAction = fileMenu->addAction(tr("Open...(&O)"), this, &MainWindow::openFile, QKeySequence::Open);
    clearAction = fileMenu->addAction(tr("Clear(&C)"), this, &MainWindow::clearCurrent, QKeySequence(Qt::CTRL | Qt::Key_L));
    fileMenu->addSeparator();
    exitAction = fileMenu->addAction(tr("Exit(&X)"), this, &QWidget::close, QKeySequence::Quit);

    auto* helpMenu = menuBar()->addMenu(tr("Help(&H)"));
    aboutAction = helpMenu->addAction(tr("About(&A)"), this, &MainWindow::showAbout);
}

void MainWindow::createToolBar()
{
    auto* mainToolBar = addToolBar(tr("Main Toolbar"));
    mainToolBar->setMovable(false);
    mainToolBar->addAction(openAction);
    mainToolBar->addAction(clearAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}