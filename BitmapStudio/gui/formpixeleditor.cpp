#include "gui/formpixeleditor.h"
#include "ui_formpixeleditor.h"
#include <QGraphicsRectItem>
#include <QDebug>
#include <global.h>
#include <QScrollBar>
#include <QCursor>
#include <custom/qcustommenu.h>
#include "gui/dialogresize.h"


FormPixelEditor::FormPixelEditor(QWidget *parent) :
    CustomTab(parent),
    ui(new Ui::FormPixelEditor)
{
    ui->setupUi(this);

    //QRectF rect(0, 0, image->width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, image->height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset);
    scene = new QGraphicsScene(QRectF(0, 0, 1, 1));
    ui->graphicsView->setScene(scene);

    scaleItem = new QGraphicsScaleItem(ui->graphicsView);
    scaleItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    scanvasItem = new QGraphicsCanvasItem(ui->graphicsView);
    scanvasItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    scene->addItem(scanvasItem);
    scene->addItem(scaleItem);

    QBrush brush;
    brush.setColor(0xcbd4e4);
    brush.setStyle(Qt::SolidPattern);
    scene->setBackgroundBrush(brush);

    ui->graphicsView->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);

    connect(ui->graphicsView, SIGNAL(paint()), this, SLOT(paintView()));    // todo Lambda
    connect(ui->actReserve, SIGNAL(triggered()), scanvasItem, SLOT(on_Reserve()));
    connect(ui->actCenter, SIGNAL(triggered()), scanvasItem, SLOT(on_AlignCenter()));
    connect(ui->actAlignHCenter, SIGNAL(triggered()), scanvasItem, SLOT(on_AlignHCenter()));
    connect(ui->actAlignVCenter, SIGNAL(triggered()), scanvasItem, SLOT(on_AlignVCenter()));
    connect(ui->actAutoResize, SIGNAL(triggered()), scanvasItem, SLOT(on_AutoResize()));
    connect(ui->actMoveUp, SIGNAL(triggered()), scanvasItem, SLOT(on_MoveUp()));
    connect(ui->actMoveDown, SIGNAL(triggered()), scanvasItem, SLOT(on_MoveDown()));
    connect(ui->actMoveLeft, SIGNAL(triggered()), scanvasItem, SLOT(on_MoveLeft()));
    connect(ui->actMoveRight, SIGNAL(triggered()), scanvasItem, SLOT(on_MoveRight()));
    connect(ui->actFlipHorizontal, SIGNAL(triggered()), scanvasItem, SLOT(on_FlipHor()));
    connect(ui->actFlipVerital, SIGNAL(triggered()), scanvasItem, SLOT(on_FlipVer()));
    connect(ui->actRotateLeft, SIGNAL(triggered()), scanvasItem, SLOT(on_RotateLeft()));
    connect(ui->actRotateRight, SIGNAL(triggered()), scanvasItem, SLOT(on_RotateRight()));

    connect(this->scanvasItem, SIGNAL(updateStatusBarPos(QPoint)), this->parent()->parent()->parent(), SLOT(on_UpdateStatusBarPos(QPoint)));
    connect(this->scanvasItem, SIGNAL(updateStatusBarSize(QSize)), this->parent()->parent()->parent(), SLOT(on_UpdateStatusBarSize(QSize)));
    connect(this->scanvasItem, SIGNAL(updatePreview(QImage)), this->parent()->parent()->parent(), SLOT(on_UpdatePreview(QImage)));

    connect(this, SIGNAL(saveImage(QString,int,QImage)), this->parent()->parent()->parent(), SLOT(on_SaveImage(QString, int, QImage)));

    connect(this->scanvasItem, &QGraphicsCanvasItem::changed,  [=](bool unsaved){
        emit changed(getProject(), getId(), unsaved);
    });


    addAction(ui->actMoveUp);
    addAction(ui->actMoveDown);
    addAction(ui->actMoveLeft);
    addAction(ui->actMoveRight);
}

FormPixelEditor::~FormPixelEditor()
{
    delete scaleItem;
    delete scanvasItem;
    delete scene;

    delete ui;
}

void FormPixelEditor::save()
{
    emit saveImage(getProject(), getId(), scanvasItem->getImage());
    emit changed(getProject(), getId(), false);
}





void FormPixelEditor::initScrollerPos()
{
    // 将scene的初始滚动条位置设置为左上角
    // 必须要QGraphicsView绘制过一次后，重新设置滚轮位置才有效
    static bool flag = true;
    if(flag)
    {
        flag = false;
        ui->graphicsView->horizontalScrollBar()->setSliderPosition(0);
        ui->graphicsView->verticalScrollBar()->setSliderPosition(0);
    }
}

void FormPixelEditor::initAction()
{
//    ui->toolBtnRotateFlip->setIcon(QIcon(":/Image/PixelEditor/FlipHorizontal.svg"));
//    QCustomMenu *menu = new QCustomMenu(this);
//    menu->addAction(ui->actFlipHorizontal);
//    menu->addAction(ui->actFlipVerital);
//    menu->addAction(ui->actRotateLeft);
//    menu->addAction(ui->actRotateRight);
//    ui->toolBtnRotateFlip->setMenu(menu);
//    ui->toolBtnRotateFlip->setPopupMode(QToolButton::InstantPopup);

//    ui->toolBtnMove->setIcon(QIcon(":/Image/PixelEditor/Move.svg"));
//    QCustomMenu *menuMove = new QCustomMenu(this);
//    menuMove->addAction(ui->actMoveUp);
//    menuMove->addAction(ui->actMoveDown);
//    menuMove->addAction(ui->actMoveLeft);
//    menuMove->addAction(ui->actMoveRight);
//    menuMove->setToolTip("移动");
//    ui->toolBtnMove->setMenu(menuMove);
//    ui->toolBtnMove->setPopupMode(QToolButton::InstantPopup);

//    ui->toolBtnReserve->setDefaultAction(ui->actReserve);
//    ui->toolBtnCenter->setDefaultAction(ui->actCenter);
//    ui->toolBtnAutoResize->setDefaultAction(ui->actAutoResize);
//    ui->toolBntSelect->setDefaultAction(ui->actSelect);
//    ui->toolBntEdit->setDefaultAction(ui->actEdit);
//    ui->toolBntMeasure->setDefaultAction(ui->actMeasure);
//    ui->toolBtnSave->setDefaultAction(ui->actSave);
}

void FormPixelEditor::leaveEvent(QEvent *event)
{
    emit updateStatusBarPos(QPoint(-1, -1));
}

void FormPixelEditor::contextMenuEvent(QContextMenuEvent *event)
{
    // 编辑模式下右键无效
    if(Global::editMode)
        return;

    QMenu menu;

    menu.addAction(ui->actAutoResize);

    QMenu menuRotate(tr("变换"));
    menuRotate.addAction(ui->actRotateLeft);
    menuRotate.addAction(ui->actRotateRight);
    menuRotate.addAction(ui->actFlipHorizontal);
    menuRotate.addAction(ui->actFlipVerital);
    menu.addMenu(&menuRotate);

    QCustomMenu menuMove;
    menuMove.setTitle(tr("移动"));
    menuMove.setIcon(QIcon(":/Image/PixelEditor/Move.svg"));
    menuMove.addAction(ui->actMoveUp);
    menuMove.addAction(ui->actMoveDown);
    menuMove.addAction(ui->actMoveLeft);
    menuMove.addAction(ui->actMoveRight);
    menu.addMenu(&menuMove);

    QMenu menuAuxiliaryLine(tr("辅助线"));
    menuAuxiliaryLine.addAction(ui->actLockAuxiliaryLine);
    menuAuxiliaryLine.addAction(ui->actHideAuxiliaryLine);
    menuAuxiliaryLine.addAction(ui->actClearAuxiliaryLines);
    menu.addMenu(&menuAuxiliaryLine);

    QMenu menuAlign(tr("布局"));
    menuAlign.addAction(ui->actCenter);
    menuAlign.addAction(ui->actAlignHCenter);
    menuAlign.addAction(ui->actAlignVCenter);
    menu.addMenu(&menuAlign);

    menu.addAction(ui->actResize);

    menu.addAction(ui->actReserve);
    menu.exec(QCursor::pos());
}

void FormPixelEditor::paintView()
{
    initScrollerPos();
}

void FormPixelEditor::on_LoadImage(QImage &image)
{
    scanvasItem->setImage(image);
    setSize(image.size());
}




void FormPixelEditor::on_actLockAuxiliaryLine_triggered()
{
    QGraphicsCanvasItem::AuxiliaryLine::lock = !QGraphicsCanvasItem::AuxiliaryLine::lock;
    if(QGraphicsCanvasItem::AuxiliaryLine::lock)
    {
        ui->actLockAuxiliaryLine->setText(tr("解锁"));
        ui->actLockAuxiliaryLine->setIcon(QIcon(":/Image/PixelEditor/Unlock.svg"));
    }
    else
    {
        ui->actLockAuxiliaryLine->setText(tr("锁定"));
        ui->actLockAuxiliaryLine->setIcon(QIcon(":/Image/PixelEditor/Lock.svg"));
    }
}


void FormPixelEditor::on_actHideAuxiliaryLine_triggered()
{
    QGraphicsCanvasItem::AuxiliaryLine::hide = !QGraphicsCanvasItem::AuxiliaryLine::hide;
    if(QGraphicsCanvasItem::AuxiliaryLine::hide)
    {
        ui->actHideAuxiliaryLine->setText(tr("显示"));
        ui->actHideAuxiliaryLine->setIcon(QIcon(":/Image/PixelEditor/Visible.svg"));
    }
    else
    {
        ui->actHideAuxiliaryLine->setText(tr("隐藏"));
        ui->actHideAuxiliaryLine->setIcon(QIcon(":/Image/PixelEditor/Invisible.svg"));
    }
}


void FormPixelEditor::on_actClearAuxiliaryLines_triggered()
{
    QGraphicsCanvasItem::auxiliaryLines.clear();
}



void FormPixelEditor::on_actResize_triggered()
{
    DialogResize *dlgResize = new DialogResize(this);
    dlgResize->setDefaultSize(scanvasItem->getImage().size());
    int ret = dlgResize->exec();
    if(ret == QDialog::Accepted)
    {
        QSize size = dlgResize->getSize();
        scanvasItem->resize(size);
    }
    delete dlgResize;
}

