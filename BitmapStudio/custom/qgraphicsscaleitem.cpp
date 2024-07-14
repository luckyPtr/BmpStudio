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
#include <QDebug>

// 辅助线
QVector<QGraphicsScaleItem::AuxiliaryLine> QGraphicsScaleItem::auxiliaryLines;
bool QGraphicsScaleItem::isLock;
bool QGraphicsScaleItem::isHide;

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
    if (auxiliaryLines.size() == 0 || QGraphicsScaleItem::isHide)
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
                painter->drawLine(0, startPoint.y() + scale * Global::pixelSize, width, startPoint.y() + scale * Global::pixelSize);
            }
            else
            {
                painter->drawLine(startPoint.x() + scale * Global::pixelSize, 0, startPoint.x() + scale * Global::pixelSize, height);
            }
        }
    };

    foreach(auto line, auxiliaryLines)
    {
        paintLine(line.dir, line.scale);
    }

    if (!QGraphicsScaleItem::isLock)
    {
        int nearLine = getNearGuide(mousePos.toPoint());
        if (selectedAuxiliaryLine != -1)
        {
            nearLine = selectedAuxiliaryLine;
        }
        if (nearLine != -1)
        {
            AuxiliaryLine line = auxiliaryLines.at(nearLine);
            pen.setColor(selectedGuidesColor);
            painter->setPen(pen);
            paintLine(line.dir, line.scale);
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
            if (line.dir == Qt::Horizontal)
            {
                int y = startPoint.y() + Global::pixelSize * line.scale;
                if (qAbs(y - point.y()) <= Global::pixelSize / 4)
                {
                    return i;
                }
            }
            else
            {
                int x = startPoint.x() + Global::pixelSize * line.scale;
                if (qAbs(x - point.x()) <= Global::pixelSize / 4)
                {
                    return i;
                }
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
            scene()->sendEvent(item, event);
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

    this->setFlag(QGraphicsItem::ItemIsFocusable, false);
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

    drawAuxiliaryLines(painter);
    drawScale(painter);
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

    }

    this->update();
}

void QGraphicsScaleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    selectedAuxiliaryLine = getNearGuide(event->pos().toPoint());

    // 非辅助线区域，传递下去
    if (selectedAuxiliaryLine == -1 || QGraphicsScaleItem::isLock || QGraphicsScaleItem::isHide)
    {
        sendEventToOtherItems(event);
    }

    if (createGuidesStep == 0)
    {
        QPoint point = event->pos().toPoint();
        if (isHorizontalScale(point))
        {
            createGuidesStep = 1;
        }
        else if (isVerticalScale(point))
        {
            createGuidesStep = 2;
        }
    }

    mouseButton = event->button();
}

void QGraphicsScaleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint currentPoint = event->pos().toPoint();

    if (mouseButton != Qt::LeftButton)
    {
        sendEventToOtherItems(event);
        return;
    }

    auto createAuxiliaryLine = [=](Qt::Orientation ori) {
        AuxiliaryLine auxLine(ori, 0);
        auxiliaryLines << auxLine;
        selectedAuxiliaryLine = auxiliaryLines.size() - 1;
        view->setCursor(auxLine.dir == Qt::Horizontal ? Qt::SplitVCursor : Qt::SplitHCursor);
    };

    if (!QGraphicsScaleItem::isLock && !QGraphicsScaleItem::isHide)
    {
        // 有辅助线被选中
        if (selectedAuxiliaryLine >= 0)
        {
            AuxiliaryLine *line = &auxiliaryLines[selectedAuxiliaryLine];
            QPoint point = pointToPixel(currentPoint);
            line->scale = line->dir == Qt::Horizontal ? point.y() : point.x();

            if (line->scale < 0)
                view->setCursor(Qt::ForbiddenCursor);
            else
                view->setCursor(line->dir == Qt::Horizontal ? Qt::SplitVCursor : Qt::SplitHCursor);
        }
        else
        {
            // 从刻度区域移动到画布区域
            if (!isScaleArea(currentPoint))
            {
                if (createGuidesStep == 1)
                {
                    createAuxiliaryLine(Qt::Horizontal);
                }
                else if (createGuidesStep == 2)
                {
                    createAuxiliaryLine(Qt::Vertical);
                }
                else
                {
                    sendEventToOtherItems(event);
                }
            }
            else
            {
                if (getNearGuide(event->pos().toPoint()) == -1)
                {
                    sendEventToOtherItems(event);
                }
            }
        }
    }
    else
    {
        sendEventToOtherItems(event);
    }
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
        view->setCursor(Qt::ArrowCursor);
    }
    selectedAuxiliaryLine = -1;
    createGuidesStep = 0;

    //if (getNearGuide(event->pos().toPoint()) == -1)
    {
        sendEventToOtherItems(event);
    }

    mouseButton = Qt::NoButton;
}

void QGraphicsScaleItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QPoint point = event->pos().toPoint();

    // 在标尺上右键
    if (isScaleArea(point))
    {
        QMenu menu;
        QAction *actDeleteAllGuides = menu.addAction(tr("删除所有参考线"));
        QAction *actLockGuides = menu.addAction(tr(QGraphicsScaleItem::isLock ? "解锁参考线" : "锁定参考线"));
        QAction *actHideGuides = menu.addAction(tr(QGraphicsScaleItem::isHide ? "显示参考线" : "隐藏参考线"));

        connect(actDeleteAllGuides, &QAction::triggered, this, &QGraphicsScaleItem::on_DeleteAllGuides);
        connect(actLockGuides, &QAction::triggered, this, &QGraphicsScaleItem::on_LockGuides);
        connect(actHideGuides, &QAction::triggered, this, &QGraphicsScaleItem::on_HideGuides);

        menu.exec(event->screenPos());
    }
    else if (getNearGuide(point) != -1 && !QGraphicsScaleItem::isLock && !QGraphicsScaleItem::isHide)
    {
        QMenu menu;
        QAction *actDelteGuide = menu.addAction(tr("删除"));
        connect(actDelteGuide, &QAction::triggered, this, &QGraphicsScaleItem::on_DeleteGuide);

        // 在鼠标右键点击的位置显示菜单
        menu.exec(event->screenPos());
    }
    else
    {
        QList<QGraphicsItem *> items = scene()->items();
        foreach (auto item, items)
        {
            if (item != this)
            {
                scene()->sendEvent(item, event);
            }
        }
    }
}

void QGraphicsScaleItem::on_DeleteAllGuides()
{
    auxiliaryLines.clear();
}

void QGraphicsScaleItem::on_LockGuides()
{
    QGraphicsScaleItem::isLock = !QGraphicsScaleItem::isLock;
}

void QGraphicsScaleItem::on_HideGuides()
{
    QGraphicsScaleItem::isHide = !QGraphicsScaleItem::isHide;
}

void QGraphicsScaleItem::on_DeleteGuide()
{
    int guideIndex = getNearGuide(mousePos.toPoint());
    if (guideIndex != -1)
        auxiliaryLines.removeAt(guideIndex);
}
