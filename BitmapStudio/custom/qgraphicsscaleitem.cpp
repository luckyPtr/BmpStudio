#include "qgraphicsscaleitem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "qwgraphicsview.h"
#include <QDebug>
#include "global.h"
#include <QScrollBar>
#include <QGraphicsSceneMouseEvent>


void QGraphicsScaleItem::drawScale(QPainter *painter)
{
    // 绘制轮廓和背景
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(142, 156, 175));
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    QBrush brush;
    brush.setColor(QColor(241, 243, 248));
    brush.setStyle(Qt::SolidPattern);
    painter->setBrush(brush);

    int width = qMax(view->width(), (int)view->scene()->width());
    int height = qMax(view->height(), (int)view->scene()->height());

    int x0 = view->horizontalScrollBar()->value();
    int y0 = view->verticalScrollBar()->value();

    QPoint points[] = {
        QPoint(-1 + x0, -4 + y0),
        QPoint(width + x0, -4 + y0),
        QPoint(width + x0, Global::scaleWidth + y0),
        QPoint(Global::scaleWidth + x0, Global::scaleWidth + y0),
        QPoint(Global::scaleWidth + x0, height + y0),
        QPoint(0 + x0, height + y0)
    };
    painter->drawPolygon(points, 6);

    // 绘制刻度线
    // 水平刻度线
    for(int i = 0, x = 0; x < width; i++)
    {
        x = Global::scaleWidth + Global::scaleOffset + i * Global::pixelSize;
        QLine line(x, i % 10 == 0 ? y0 : Global::scaleWidth - 4 + y0, x, Global::scaleWidth + y0);
        painter->drawLine(line);
    }
    // 垂直刻度线
    for(int i = 0, y = 0; y < height; i++)
    {
        y = Global::scaleWidth + Global::scaleOffset + i * Global::pixelSize;
        QLine line(i % 10 == 0 ? x0 : Global::scaleWidth - 4 + x0, y, Global::scaleWidth + x0, y);
        painter->drawLine(line);
    }

    // 绘制刻度线数字
    QFont font(QStringLiteral("微软雅黑"), 8);
    painter->setFont(font);
    pen.setColor(QColor(0x334b6a));
    painter->setPen(pen);

    for(int i = 0, x = 0; x < width; i+=10)
    {
        x = Global::scaleWidth + Global::scaleOffset + i * Global::pixelSize;
        // 每10像素绘制长刻度线和数值
        painter->drawText(x + 2, Global::scaleWidth / 2 + 2 + y0, QString::number(i));
    }
    painter->save();
    painter->rotate(90);
    for(int i = 0, y = 0; y < height; i+=10)
    {
        y = Global::scaleWidth + Global::scaleOffset + i * Global::pixelSize;
        painter->drawText(y + 2, Global::scaleWidth / 2 - 10 - x0, QString::number(i));
    }
    painter->restore();

    // 绘制当前刻度
    pen.setColor(Qt::red);
    painter->setPen(pen);
    QLine lineHorizontalScale(qMax((int)mousePos.x(), Global::scaleWidth), 0, qMax((int)mousePos.x(), Global::scaleWidth), Global::scaleWidth);
    QLine lineVerticalScale(0, qMax((int)mousePos.y(), Global::scaleWidth), Global::scaleWidth, qMax((int)mousePos.y(), Global::scaleWidth));
    painter->drawLine(lineHorizontalScale);
    painter->drawLine(lineVerticalScale);
}


QGraphicsScaleItem::QGraphicsScaleItem(QWidget *parent)
{
    view = static_cast<QWGraphicsView*>(parent);
    connect(view, SIGNAL(mouseMovePoint(QPoint)), this, SLOT(mouseMove(QPoint)));

    view->setStyleSheet("QGraphicsView {border:1px solid #A0A0A0; border-top:none}");
}

QGraphicsScaleItem::~QGraphicsScaleItem()
{

}

QRectF QGraphicsScaleItem::boundingRect() const
{
    int width = qMax(view->width(), (int)view->scene()->width());
    int height = qMax(view->height(), (int)view->scene()->height());
    return QRectF(0, 0, width, height);
}

void QGraphicsScaleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    drawScale(painter);
}

QPainterPath QGraphicsScaleItem::shape() const
{
    int width = qMax(view->width(), (int)view->scene()->width());
    int height = qMax(view->height(), (int)view->scene()->height());
    QPainterPath path;

    QVector<QPointF> points = {
        QPointF(0, 0),
        QPointF(width, 0),
        QPointF(width, Global::scaleWidth),
        QPointF(Global::scaleWidth, Global::scaleWidth),
        QPointF(Global::scaleWidth, height),
        QPointF(0, height)
    };
    path.addPolygon(QPolygonF(points));

    return path;
}

void QGraphicsScaleItem::mouseMove(QPoint point)
{
    // 将坐标映射到sense原点相对位置
    mousePos = point;
    mousePos.setX(mousePos.x() + view->horizontalScrollBar()->value());
    mousePos.setY(mousePos.y() + view->verticalScrollBar()->value());
    this->update();
}

void QGraphicsScaleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    static QPointF lastPoint(520, 520);
    QPointF currentPoint = event->pos();

    if(lastPoint.x() <= Global::scaleWidth && currentPoint.x() > Global::scaleWidth)
    {
        if(!createFlag)
        {
            createFlag = true;
            emit createAuxLine(Qt::Vertical);
            qDebug() << "emit createAuxLine(Qt::Vertical);";
        }

    }
    else if(lastPoint.y() <= Global::scaleWidth && currentPoint.y() > Global::scaleWidth)
    {
        if(!createFlag)
        {
            createFlag = true;
            emit createAuxLine(Qt::Horizontal);
            qDebug() << "emit createAuxLine(Qt::Horizontal);";
        }
    }

    lastPoint = currentPoint;
}

void QGraphicsScaleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    createFlag = false;
}
