#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include <QFileDialog>
#include <QMessageBox>
#include <custom/treemodel.h>
#include <custom/treeitem.h>
#include <core/rawdata.h>
#include <QDialog>
#include <QInputDialog>
#include <QPainter>
#include <QToolButton>
#include <gui/dialognewimgfile.h>
#include <gui/dialogimportimg.h>
#include <gui/formpixeleditor.h>
#include <gui/formcomimgeditor.h>
#include <gui/dialogabout.h>
#include <gui/dialognotice.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QClipboard>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();

    connect(QApplication::instance(), SIGNAL(importFile(QString)), SLOT(on_ImportFile(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::init()
{
    initStatusBar();
    initAction();
    treeItemDelegate = new TreeItemDelegate();
    ui->treeViewProject->setItemDelegate(treeItemDelegate);
    pm.blindTreeView(ui->treeViewProject);
    pm.setTabWidget(ui->tabWidget);
    ui->lineEditSearch->setVisible(false);

    Global::initSettings();
}

void MainWindow::initStatusBar()
{
    labelPosition = new QLabel();
    labelSize = new QLabel();
    labelPositionIco = new QLabel();
    labelSizeIco_new = new QLabel();
    labelMode = new QLabel();

    labelPositionIco->setPixmap(QPixmap(":/Image/StatusBar/Position.png"));
    labelSizeIco_new->setPixmap(QPixmap(":/Image/StatusBar/Size.png"));

    ui->statusbar->addWidget(labelPositionIco);
    ui->statusbar->addWidget(labelPosition);
    ui->statusbar->addWidget(labelSizeIco_new);
    ui->statusbar->addWidget(labelSize);
    ui->statusbar->addPermanentWidget(labelMode);

    labelPosition->setMinimumWidth(120);
    labelSize->setMinimumWidth(120);
}

void MainWindow::initAction()
{
    connect(ui->actRun, SIGNAL(triggered()), &pm, SLOT(on_ActRun_Triggered()));
    connect(ui->actNewProject, SIGNAL(triggered()), &pm, SLOT(on_ActNewProject_Triggered()));
    connect(ui->actOpenProject, SIGNAL(triggered()), &pm, SLOT(on_ActOpenProject_Triggered()));
    connect(&pm, SIGNAL(updateSelectProject(QString)), this, SLOT(on_SelectedProjectChanged(QString)));
    connect(ui->actSave, SIGNAL(triggered()), ui->tabWidget, SLOT(on_ActSave_Triggered()));
    connect(ui->actSaveAll, SIGNAL(triggered()), ui->tabWidget, SLOT(on_ActSaveAll_Triggered()));
    connect(ui->treeViewProject, SIGNAL(openProject(QString)), &pm, SLOT(on_OpenProjectUrl(QString)));
    connect(ui->treeViewProject, SIGNAL(importImageFile(QModelIndex,QString)), &pm, SLOT(on_ImportImg(QModelIndex,QString)));
}



void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)

    if(!imagePreview.isNull())
    {
        ui->labelPreview->clear();
        QImage resultImg = imagePreview.scaled(ui->labelPreview->size(), Qt::KeepAspectRatio);
        ui->labelPreview->setPixmap(QPixmap::fromImage(resultImg));
    }
}



void MainWindow::on_SaveImage(QImage image)
{
    pm.setImage(editedIndex, image);
}

void MainWindow::on_SaveComImg(ComImg comImg)
{
    pm.setComImg(editedIndex, comImg);
}

void MainWindow::on_SaveImage(QString project, int id, QImage image)
{
    QModelIndex index = pm.getModelIndex(project, id);
    if(index.isValid())
    {
        pm.setImage(index, image);
    }
}

void MainWindow::on_SaveComImg(QString project, int id, ComImg comImg)
{
    QModelIndex index = pm.getModelIndex(project, id);
    if(index.isValid())
    {
        pm.setComImg(index, comImg);
    }
}

void MainWindow::on_OpenImgTab(QString project, int id)
{
    QModelIndex index = pm.getModelIndex(project, id);
    if (index.isValid())
    {
        TreeItem *item = pm.model()->itemFromIndex(index);
        ui->tabWidget->openTab(item);
    }
}

void MainWindow::on_ImportFile(QString filePath)
{
    if (!filePath.isEmpty())
    {
        pm.on_OpenProjectUrl(filePath);

        this->setWindowState(Qt::WindowActive);
        this->raise();
    }
}



void MainWindow::on_UpdateStatusBarPos(QPoint point)
{
    if(point.x() >= 0 && point.y() >= 0)
    {
        labelPosition->setText(QString(tr("%1, %2像素")).arg(point.x()).arg(point.y()));
    }
    else
    {
        labelPosition->clear();
    }
}

void MainWindow::on_UpdateStatusBarSize(QSize size)
{
    if(size.width() >= 0 && size.height() >= 0)
    {
        labelSize->setText(QString(tr("%1 x %2像素")).arg(size.width()).arg(size.height()));
    }
    else
    {
        labelSize->clear();
    }
}


void MainWindow::on_UpdatePreview(QImage image)
{
    if(!image.isNull())
    {
        imagePreview = image;
        QImage resultImg = image.scaled(ui->labelPreview->size(), Qt::KeepAspectRatio);
        ui->labelPreview->setPixmap(QPixmap::fromImage(resultImg));
    }
}




void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}




void MainWindow::on_actEditMode_triggered(bool checked)
{
    Global::editMode = checked;
    labelMode->setText(tr(checked ? "编辑模式" : ""));
    // DialogNotice *dlg = new DialogNotice(checked ? tr("像素编辑模式") : tr("普通模式"));
    // dlg->exec();
}


void MainWindow::on_treeViewProject_doubleClicked(const QModelIndex &index)
{
    TreeItem *item = pm.model()->itemFromIndex(index);
    ui->tabWidget->openTab(item);
}

void MainWindow::on_SelectedProjectChanged(QString project)
{
    if(!project.isEmpty())
    {
        QFileInfo fileInfo(project);
        ui->actRun->setEnabled(true);
        ui->actRun->setToolTip(QString("运行项目\"%1\"").arg(fileInfo.baseName()));
        this->setWindowTitle(QString("%1 - %2").arg(fileInfo.baseName()).arg(qApp->applicationName()));
    }
    else
    {
        ui->actRun->setEnabled(false);
        ui->actRun->setToolTip(tr("运行"));
        this->setWindowTitle(qApp->applicationName());
    }
}

void MainWindow::on_UpdateSaveStatus(bool enableSave, bool enableSaveAll)
{
    ui->actSave->setEnabled(enableSave);
    ui->actSaveAll->setEnabled(enableSaveAll);
}



void MainWindow::on_actAbout_triggered()
{
    DialogAbout *dlg = new DialogAbout(this);
    dlg->exec();
    delete dlg;
}



