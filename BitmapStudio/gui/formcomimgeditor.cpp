#include "formcomimgeditor.h"
#include "ui_formcomimgeditor.h"
#include <QScrollBar>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QMenu>
#include <QShortcut>
#include <QScreen>
#include <custom/qcustommenu.h>
#include <gui/dialogresize.h>
#include <gui/dialogposition.h>
#include "global.h"

class FixedScaleView : public QGraphicsView {
public:
    FixedScaleView(QGraphicsScene* scene, QWidget* parent = nullptr)
        : QGraphicsView(scene, parent) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QGraphicsView::resizeEvent(event);
        fitInView(scene()->sceneRect(), Qt::IgnoreAspectRatio);
    }
};



FormComImgEditor::FormComImgEditor(QWidget *parent) :
    CustomTab(parent),
    ui(new Ui::FormComImgEditor)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(QRectF(0, 0, 1, 1));
    ui->graphicsView->setScene(scene);

    scaleItem = new QGraphicsScaleItem(ui->graphicsView);
    scaleItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    comImgCanvansItem = new QGraphicsComImgCanvansItem(ui->graphicsView);
    comImgCanvansItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    scene->addItem(comImgCanvansItem);
    scene->addItem(scaleItem);


    // 获取屏幕的大小
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    // 创建一个 QLinearGradient 对象，从左上角到右下角渐变
    QLinearGradient gradient(0, 0, screenWidth, screenHeight);
    // 设置渐变的颜色，从左上角的浅蓝色到右下角的深蓝色
    gradient.setColorAt(0, QColor(Global::backgroundColorLeftTop));  // 浅蓝色
    gradient.setColorAt(1, QColor(Global::backgroundColorRightBottom));  // 深蓝色
    // 创建一个 QBrush 并使用渐变色
    QBrush brush(gradient);
    scene->setBackgroundBrush(brush);

    ui->graphicsView->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);




    connect(this->comImgCanvansItem, SIGNAL(updateStatusBarPos(QPoint)), this->parent()->parent()->parent(), SLOT(on_UpdateStatusBarPos(QPoint)));
    connect(this->comImgCanvansItem, SIGNAL(updateStatusBarSize(QSize)), this->parent()->parent()->parent(), SLOT(on_UpdateStatusBarSize(QSize)));
    connect(this->comImgCanvansItem, SIGNAL(updatePreview(QImage)), this->parent()->parent()->parent(), SLOT(on_UpdatePreview(QImage)));

    connect(this, SIGNAL(saveComImg(QString,int,ComImg)), this->parent()->parent()->parent(), SLOT(on_SaveComImg(QString,int,ComImg)));
    connect(this, SIGNAL(openImgTab(QString, int)), this->parent()->parent()->parent(), SLOT(on_OpenImgTab(QString, int)));

    connect(this->comImgCanvansItem, &QGraphicsComImgCanvansItem::changed,  [=](bool unsaved){
        emit changed(getProject(), getId(), unsaved);
    });

    initShortCut();
}

FormComImgEditor::~FormComImgEditor()
{
    delete comImgCanvansItem;
    delete scaleItem;
    delete scene;
    delete ui;
}

void FormComImgEditor::save()
{
    emit saveComImg(getProject(), getId(), comImgCanvansItem->getComImg());
    emit changed(getProject(), getId(), false);
}

void FormComImgEditor::initScrollerPos()
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

void FormComImgEditor::initShortCut()
{
    auto createShortCut = [=](auto key, void (QGraphicsComImgCanvansItem::*method)()) {
        QShortcut* shortcut = new QShortcut(QKeySequence(key), this);
        connect(shortcut, &QShortcut::activated, comImgCanvansItem, method);
    };

    createShortCut("Up", &QGraphicsComImgCanvansItem::on_MoveUp);
    createShortCut("Down", &QGraphicsComImgCanvansItem::on_MoveDown);
    createShortCut("Left", &QGraphicsComImgCanvansItem::on_MoveLeft);
    createShortCut("Right", &QGraphicsComImgCanvansItem::on_MoveRight);
    createShortCut("Home", &QGraphicsComImgCanvansItem::on_RaiseToTop);
    createShortCut("End", &QGraphicsComImgCanvansItem::on_LowerToBottom);
    createShortCut("PgUp", &QGraphicsComImgCanvansItem::on_Raise);
    createShortCut("PgDown", &QGraphicsComImgCanvansItem::on_Lower);
    createShortCut("Ctrl+Alt+V", &QGraphicsComImgCanvansItem::on_AlignVCenter);
    createShortCut("Ctrl+Alt+H", &QGraphicsComImgCanvansItem::on_AlignHCenter);
    createShortCut("Ctrl+Alt+C", &QGraphicsComImgCanvansItem::on_AlignCenter);
    createShortCut("Delete", &QGraphicsComImgCanvansItem::on_DeleteSelectItem);
    createShortCut("Ctrl+Shift+X", &QGraphicsComImgCanvansItem::on_DeleteAll);
    createShortCut("Ctrl+A", &QGraphicsComImgCanvansItem::on_SelectAll);
    createShortCut("Tab", &QGraphicsComImgCanvansItem::on_SwitchSelectedItem);
    createShortCut("Esc", &QGraphicsComImgCanvansItem::on_SelectNone);
}




void FormComImgEditor::leaveEvent(QEvent *event)
{
    emit updateStatusBarPos(QPoint(-1, -1));
}


void FormComImgEditor::on_LoadComImg(ComImg &comImg, RawData *rd)
{
    comImgCanvansItem->setComImg(comImg);
    comImgCanvansItem->setRawData(rd);
    setSize(comImg.size);
}





