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
    , infoAction(nullptr)
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

    statusBar()->showMessage(tr("Loaded: %1").arg(fileName), 3000);

    niftiInfoReader.loadImageInfo(fileName);
    QMessageBox::information(this, tr("NIfTI Image Info"), niftiInfoReader.getInfo().isEmpty() ? tr("Failed to read image info") : niftiInfoReader.getInfo());
    niftiInfoReader.printITKImageInfo();
    niftiInfoReader.printVTKImageInfo();

    auto vtkImage = niftiInfoReader.vtkImage();
    if (vtkImage)
    {
        imageViewWidget->setVtkImageData(vtkImage);
        updateStatusBarLabels();
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("Failed to load image data"));
    }
}

void MainWindow::clearCurrent()
{
    imageViewWidget->clearView();
    niftiInfoReader.reset();
    updateStatusBarLabels();
    statusBar()->showMessage(tr("Current display cleared"), 2000);
}

void MainWindow::showInfo()
{
    if (niftiInfoReader.getInfo().isEmpty()) {
        QMessageBox::information(this, tr("Image Info"), tr("No image info available"));
    } else {
        QMessageBox::information(this, tr("Image Info"), niftiInfoReader.getInfo());
    }
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
    infoAction = helpMenu->addAction(tr("Info(&I)"), this, &MainWindow::showInfo);
}

void MainWindow::createToolBar()
{
    auto* mainToolBar = addToolBar(tr("Main Toolbar"));
    mainToolBar->setMovable(false);
    mainToolBar->addAction(openAction);
    mainToolBar->addAction(clearAction);
    mainToolBar->addAction(infoAction);
}

void MainWindow::createStatusBar()
{
    m_fileLabel      = new QLabel(tr(" has no image loaded "), this);
    m_imageSizeLabel = new QLabel(tr(" None "), this);

    // 设置对象名，方便通过 QSS 样式化
    m_fileLabel->setObjectName("statusFileLabel");
    m_imageSizeLabel->setObjectName("statusImageSizeLabel");

    auto* sepA = new QLabel(tr(" | "), this);
    sepA->setEnabled(false);

    statusBar()->addPermanentWidget(m_fileLabel);
    statusBar()->addPermanentWidget(sepA);
    statusBar()->addPermanentWidget(m_imageSizeLabel);

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::updateStatusBarLabels()
{
    const auto& shape = niftiInfoReader.shape();
    if (shape.isEmpty())
    {
        m_fileLabel->setText(tr(" has no image loaded "));
        m_imageSizeLabel->setText(tr(" None "));
        return;
    }

    // 文件名（显示完整路径）
    QString fileName = niftiInfoReader.fileName();
    m_fileLabel->setText(tr("File: %1").arg(fileName));
    m_fileLabel->setToolTip(fileName);

    // 图像尺寸 + 间距
    QString sizeStr;
    sizeStr += tr("size: ");
    for (int i = 0; i < shape.size(); ++i)
    {
        sizeStr += QString::number(shape[i]);
        if (i + 1 < shape.size()) sizeStr += " × ";
    }
    if (niftiInfoReader.spacing().size() >= 3)
    {
        sizeStr += QString("  spacing: %1 / %2 / %3 mm")
            .arg(niftiInfoReader.spacing()[0], 0, 'f', 2)
            .arg(niftiInfoReader.spacing()[1], 0, 'f', 2)
            .arg(niftiInfoReader.spacing()[2], 0, 'f', 2);
    }
    m_imageSizeLabel->setText(QString(" %1 ").arg(sizeStr));

}