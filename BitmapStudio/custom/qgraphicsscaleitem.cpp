#include "qgraphicsscaleitem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "qwgraphicsview.h"
#include <QDebug>
#include "global.h"
#include <QScrollBar>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QGraphicsScene>
#include <QMenu>

// 辅助线
QVector<QGraphicsScaleItem::AuxiliaryLine> QGraphicsScaleItem::auxiliaryLines;


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
    QFont font(QStringLiteral("Arial"), 8);
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

    // 覆盖有滚动条时左上角的刻度
    painter->setPen(Qt::NoPen);
    painter->drawRect(x0, y0, Global::scaleWidth + 1, Global::scaleWidth + 1);

    // 绘制当前刻度
    pen.setColor(Qt::red);
    painter->setPen(pen);
    int maxX = qMax((int)mousePos.x(), Global::scaleWidth + x0 + Global::scaleOffset);
    QLine lineHorizontalScale(maxX, y0, maxX, Global::scaleWidth + y0);
    int maxY = qMax((int)mousePos.y(), Global::scaleWidth + y0 + Global::scaleOffset);
    QLine lineVerticalScale(x0, maxY, Global::scaleWidth + x0, maxY);
    painter->drawLine(lineHorizontalScale);
    painter->drawLine(lineVerticalScale);


}

void QGraphicsScaleItem::drawAuxiliaryLines(QPainter *painter)
{
    if (auxiliaryLines.size() == 0)
        return;

    QColor guidesColor(Global::guidesColor);
    QColor selectedGuidesColor(Global::selectedGuidesColor);
    QPen pen(guidesColor);
    painter->setPen(pen);


    int width = qMax(view->width(), (int)view->scene()->width());
    int height = qMax(view->height(), (int)view->scene()->height());

    auto paintLine = [=](Qt::Orientation dir, int scale){
        if (scale >= 0)
        {
            if(dir == Qt::Horizontal)
            {
                painter->drawLine(startPoint.x(), startPoint.y() + scale * Global::pixelSize, width, startPoint.y() + scale * Global::pixelSize);
            }
            else
            {
                painter->drawLine(startPoint.x() + scale * Global::pixelSize, startPoint.y(), startPoint.x() + scale * Global::pixelSize, height);
            }
        }
    };

    foreach(auto line, auxiliaryLines)
    {
        paintLine(line.dir, line.scale);
    }

    QPoint mousePixel = pointToPixel(mousePos.toPoint());
    for (int i = auxiliaryLines.size() - 1; i >= 0; i--)
    {
        AuxiliaryLine line = auxiliaryLines.at(i);
        if (line.scale == (line.dir == Qt::Horizontal ? mousePixel.y() : mousePixel.x()))
        {
            pen.setColor(selectedGuidesColor);
            painter->setPen(pen);
            paintLine(line.dir, line.scale);
            break;
        }
    }


#if AUX_LINE_SCALE
    auto paintLinePos = [=](AuxiliaryLine line) {
        if(line.scale >= 0)
        {
            QPen pen(QColor(240, 240, 240, 220));
            QBrush brush;
            brush.setColor(QColor(240, 240, 240, 220));
            brush.setStyle(Qt::SolidPattern);
            painter->setBrush(brush);
            painter->setPen(pen);
            QFont font;
            font.setFamily("Arial");
            font.setPointSize(9);
            painter->setFont(font);

            QPoint p;
            if(line.dir == Qt::Horizontal)
            {
                p = QPoint(startPoint.x() + 10, startPoint.y() + line.scale * Global::pixelSize + 5);
            }
            else
            {
                p = QPoint(startPoint.x() + line.scale * Global::pixelSize + 10, startPoint.y() + 5);
            }

            QRect rect(p, QSize(30, 16));
            painter->drawRect(rect);
            pen.setColor(Global::gridColor);
            painter->setPen(pen);
            painter->drawText(rect, Qt::AlignCenter, QString::asprintf("%d", line.scale));
        }
    };

    if(selectedAuxiliaryLine != -1)
    {
        paintLinePos(auxiliaryLines.at(selectedAuxiliaryLine));
    }
#endif
}

bool QGraphicsScaleItem::isScaleArea(QPoint point)
{
    int x0 = view->horizontalScrollBar()->value();
    int y0 = view->verticalScrollBar()->value();

    QPolygon scaleArea;
    int width = qMax(view->width(), (int)view->scene()->width());
    int height = qMax(view->height(), (int)view->scene()->height());
    scaleArea << QPoint(x0, y0)
              << QPoint(x0+ width, y0)
              << QPoint(x0+ width, y0 + Global::scaleWidth)
              << QPoint(x0+ Global::scaleWidth, y0 + Global::scaleWidth)
              << QPoint(x0+ Global::scaleWidth, y0 + height)
              << QPoint(x0, y0 + height);

    return scaleArea.containsPoint(point, Qt::OddEvenFill);
}

bool QGraphicsScaleItem::isHorizontalScale(QPoint point)
{
    int x0 = view->horizontalScrollBar()->value();
    int y0 = view->verticalScrollBar()->value();
    int width = qMax(view->width(), (int)view->scene()->width());
    QRect rect(QPoint(Global::scaleWidth + x0, y0), QPoint(width + x0, Global::scaleWidth + y0));

    return rect.contains(point);
}

bool QGraphicsScaleItem::isVerticalScale(QPoint point)
{
    int x0 = view->horizontalScrollBar()->value();
    int y0 = view->verticalScrollBar()->value();
    int height = qMax(view->height(), (int)view->scene()->height());
    QRect rect(QPoint(x0, y0 + Global::scaleWidth), QPoint(x0 + Global::scaleWidth, y0 + height));

    return rect.contains(point);
}

int QGraphicsScaleItem::getNearGuide(QPoint point)
{
    if (auxiliaryLines.size())
    {
        for (int i = auxiliaryLines.size() - 1; i >= 0; i--)
        {
            AuxiliaryLine line = auxiliaryLines.at(i);
            QPoint p = pointToPixel(point);
            if (line.scale == (line.dir == Qt::Horizontal ? p.y() : p.x()))
            {
                return i;
            }
        }
    }
    return -1;
}



void QGraphicsScaleItem::sendEventToOtherItems(QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem *> items = scene()->items();
    foreach (auto item, items)
    {
        if (item != this)
        {
            QGraphicsSceneMouseEvent *newEvent = new QGraphicsSceneMouseEvent(event->type());
            newEvent->setScenePos(event->scenePos());
            newEvent->setScreenPos(event->screenPos());
            newEvent->setButton(event->button());
            newEvent->setButtons(event->buttons());
            newEvent->setModifiers(event->modifiers());
            scene()->sendEvent(item, newEvent);
        }
    }
}

QPoint QGraphicsScaleItem::pointToPixel(QPoint point)
{
    return QPoint((point.x() - startPoint.x() + Global::pixelSize / 2) / Global::pixelSize,
                  (point.y() - startPoint.y() + Global::pixelSize / 2) / Global::pixelSize);
}


QGraphicsScaleItem::QGraphicsScaleItem(QWidget *parent)
{
    view = static_cast<QWGraphicsView*>(parent);
    connect(view, SIGNAL(mouseMovePoint(QPoint)), this, SLOT(mouseMove(QPoint)));

    view->setStyleSheet("QGraphicsView {border:1px solid #A0A0A0; border-top:none}");

    startPoint.setX(Global::scaleWidth + Global::scaleOffset);
    startPoint.setY(Global::scaleWidth + Global::scaleOffset);
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
    drawAuxiliaryLines(painter);
}

// QPainterPath QGraphicsScaleItem::shape() const
// {
//     int width = qMax(view->width(), (int)view->scene()->width());
//     int height = qMax(view->height(), (int)view->scene()->height());
//     QPainterPath path;

//     QVector<QPointF> points = {
//         QPointF(0, 0),
//         QPointF(width, 0),
//         QPointF(width, Global::scaleWidth),
//         QPointF(Global::scaleWidth, Global::scaleWidth),
//         QPointF(Global::scaleWidth, height),
//         QPointF(0, height)
//     };
//     path.addPolygon(QPolygonF(points));

//     return path;
// }

void QGraphicsScaleItem::mouseMove(QPoint point)
{
    static QPoint lastMousePixel(0xffffffff, 0xffffffff);
    mousePos = point;

    mousePixel = pointToPixel(point);
    if (lastMousePixel != mousePixel)
    {
        lastMousePixel = mousePixel;

        this->update();
    }


}

void QGraphicsScaleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    selectedAuxiliaryLine = getNearGuide(event->pos().toPoint());

    // 非辅助线区域，传递下去
    if (selectedAuxiliaryLine == -1)
    {
        sendEventToOtherItems(event);
    }
}

void QGraphicsScaleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    static QPoint lastPoint(520, 520);
    QPoint currentPoint = event->pos().toPoint();

    auto createAuxiliaryLine = [=](Qt::Orientation ori) {
        AuxiliaryLine auxLine(ori, 0);
        auxiliaryLines << auxLine;
        selectedAuxiliaryLine = auxiliaryLines.size() - 1;
        view->setCursor(auxLine.dir == Qt::Horizontal ? Qt::SizeVerCursor : Qt::SizeHorCursor);
    };

    // 有辅助线被选中
    if (selectedAuxiliaryLine >= 0)
    {
        AuxiliaryLine *line = &auxiliaryLines[selectedAuxiliaryLine];
        QPoint point = pointToPixel(currentPoint);
        line->scale = line->dir == Qt::Horizontal ? point.y() : point.x();

        if (line->scale < 0)
            view->setCursor(Qt::ForbiddenCursor);
        else
            view->setCursor(line->dir == Qt::Horizontal ? Qt::SizeVerCursor : Qt::SizeHorCursor);
    }
    else
    {
        // 从刻度区域移动到画布区域
        if (isHorizontalScale(lastPoint) && !isScaleArea(currentPoint))
        {
            createAuxiliaryLine(Qt::Horizontal);

        }
        else if (isVerticalScale(lastPoint) && !isScaleArea(currentPoint))
        {
            createAuxiliaryLine(Qt::Vertical);
        }
        else
        {
            if (getNearGuide(event->pos().toPoint()) == -1)
            {
                sendEventToOtherItems(event);
            }
        }
    }


    lastPoint = currentPoint;
}

void QGraphicsScaleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // 删除拖到标尺的参考线
    if (selectedAuxiliaryLine >= 0)
    {
        if (auxiliaryLines.at(selectedAuxiliaryLine).scale < 0)
        {
            auxiliaryLines.removeAt(selectedAuxiliaryLine);
        }
    }
    selectedAuxiliaryLine = -1;

    if (getNearGuide(event->pos().toPoint()) == -1)
    {
        sendEventToOtherItems(event);
    }
}

void QGraphicsScaleItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *action1 = menu.addAction("Action 1");
    QAction *action2 = menu.addAction("Action 2");


    // 在鼠标右键点击的位置显示菜单
    menu.exec(event->screenPos());
}
