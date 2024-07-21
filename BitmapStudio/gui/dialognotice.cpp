#include "dialognotice.h"
#include "ui_dialognotice.h"
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QTimer>

DialogNotice::DialogNotice(QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNotice)
{
    ui->setupUi(this);

    ui->label->setText(text);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);

    // 居中显示
    QWidget *widget = nullptr;
    //获取程序所有的widget
    QWidgetList widgetList = QApplication::allWidgets();
    //获取父窗口指针 此处获取objectName 可根据需求自行判断
    for(int i=0; i<widgetList.length(); i++){
        if(widgetList.at(i)->objectName() == "MainWindow"){
            widget = widgetList.at(i);
        }
    }
    if(widget){

        QWidget::setParent(widget);

        //获取父窗口geometry
        QRect rect = widget->geometry();
        //计算显示原点

        QFontMetrics fm = ui->label->fontMetrics();
        int pixelsWide = fm.boundingRect(ui->label->text()).width() + 60;
        this->setFixedWidth(pixelsWide);

        int x = rect.width()/2 - this->width() / 2;
        int y = rect.height()/2 - this->height() / 2;
        this->move(x, y);
    }

    // ShowTime后开始每隔20ms降低一次透明度
    QTimer* t = new QTimer(this);//加入对象树
    t->start(ShowTime);
    connect(t, &QTimer::timeout, [=]{
        timer = startTimer(20);
    });
    alphaStepValue = (StartAlpha - EndAlpha) / (DisappearTime / 20);

    // 初始化字体和背景颜色
    backgroundColor = BackGroundColor;
    backgroundColor.setAlphaF(StartAlpha);
    ui->label->setStyleSheet(QString("color: rgba(255, 255, 255, %1);").arg(StartAlpha));
}

DialogNotice::~DialogNotice()
{
    delete ui;
}

void DialogNotice::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 反锯齿;
    painter.setBrush(QBrush(backgroundColor)); //窗体背景色
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();          //rect为绘制大小
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 8, 8);  //15为圆角角度
    //也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
    //QPainterPath painterPath;
    //painterPath.addRoundedRect(rect, 15, 15);//15为圆角角度
    //painter.drawPath(painterPath);
    QWidget::paintEvent(event);
}

void DialogNotice::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timer)
    {
        qreal alpha = backgroundColor.alphaF();

        alpha -= alphaStepValue;
        if(alpha > EndAlpha)
        { 
            backgroundColor.setAlphaF(alpha);
            ui->label->setStyleSheet(QString("color: rgba(255, 255, 255, %1);").arg(alpha));
            repaint();
        }
        else
        {
            this->close();
        }
    }
}

