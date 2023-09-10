#include "qgraphicscomimgcanvansitem.h"
#include "qwgraphicsview.h"
#include "global.h"
#include <QGraphicsSceneDragDropEvent>


void QGraphicsComImgCanvansItem::paintBackground(QPainter *painter)
{
    // 背景使用像素0的颜色填充
    QRect backgroudRect(startPoint.x(), startPoint.y(), comImg.width * Global::pixelSize, comImg.height * Global::pixelSize);
    painter->fillRect(backgroudRect, Global::pixelColor_0);


}

void QGraphicsComImgCanvansItem::paintItems(QPainter *painter)
{
    // 在指定位置绘制单个图形
    auto paintItem = ([=](int x0, int y0, QImage img){
        QRect canvasRect(startPoint.x(), startPoint.y(), comImg.width * Global::pixelSize, comImg.height * Global::pixelSize);

        QRect rect(startPoint.x() + x0 * Global::pixelSize, startPoint.y() + y0 * Global::pixelSize, Global::pixelSize * img.width(), Global::pixelSize * img.height());
        painter->fillRect(rect, Global::gridColor);
        for(int x = 0; x < img.width(); x++)
        {
            for(int y = 0; y < img.height(); y++)
            {
                QColor color = img.pixelColor(x, y);
                quint8 grayscale  = qGray(color.rgb());
                QRect rect(startPoint.x() + (x0 + x) * Global::pixelSize + 1, startPoint.y() + (y0 + y) * Global::pixelSize + 1, Global::pixelSize - 1, Global::pixelSize - 1);
                if(canvasRect.contains(rect.topLeft()))
                    painter->fillRect(rect, grayscale < 128 ? Global::pixelColor_1 : Global::pixelColor_0);
            }
        }
    });

    // 绘制图形外框
    auto paintBound = ([=](int x0, int y0, QSize size, int index){
        QPen pen;
        pen.setColor(index == selectedItemIndex ? Global::selectedItemBoundColor : Global::itemBoundColor);
        pen.setWidth(2);
        painter->setPen(pen);

        QRect rect(startPoint.x() + x0 * Global::pixelSize, startPoint.y() + y0 * Global::pixelSize, Global::pixelSize * size.width() + 1, Global::pixelSize * size.height() + 1);
        painter->drawRect(rect);

        // 绘制选中图形
        if(index == selectedItemIndex)
        {
            // 选择的图形高亮
            QBrush brush(QColor(0, 255, 255, 16));
            painter->fillRect(rect, brush);
            QPoint pointTopMiddle(startPoint.x() + x0 * Global::pixelSize + size.width() * Global::pixelSize / 2 - 1, startPoint.y() + y0 * Global::pixelSize - 3);
            QPoint pointBottomMiddle(pointTopMiddle.x(), pointTopMiddle.y() + size.height() * Global::pixelSize + 5);
            QPoint pointLeftMiddle(startPoint.x() + x0 * Global::pixelSize - 3, startPoint.y() + y0 * Global::pixelSize + size.height() * Global::pixelSize / 2 - 1);
            QPoint pointRightMiddle(pointLeftMiddle.x() + size.width() * Global::pixelSize + 5, pointLeftMiddle.y());
            // 四边中点
            brush.setColor(Global::selectedItemBoundColor);
            painter->fillRect(QRect(pointTopMiddle, QSize(3, 3)), brush);
            painter->fillRect(QRect(pointBottomMiddle, QSize(3, 3)), brush);
            painter->fillRect(QRect(pointLeftMiddle, QSize(3, 3)), brush);
            painter->fillRect(QRect(pointRightMiddle, QSize(3, 3)), brush);
            //painter->drawLine(pointLeftMiddle, QPoint(pointLeftMiddle.x() + 200, pointLeftMiddle.y()));
        }
    });

    for(int i = 0; i < comImg.items.size(); i++)
    {
        ComImgItem item = comImg.items[i];
        QImage img = rd->getImage(item.id);
        paintItem(item.x, item.y, img);
        paintBound(item.x, item.y, img.size(), i);
    }
}

void QGraphicsComImgCanvansItem::paintGrid(QPainter *painter)
{
    // 网格
    QPen pen(Global::gridColor);
    painter->setPen(pen);
    for(int x = 0; x < comImg.width; x++)
    {
        painter->drawLine(startPoint.x() + x * Global::pixelSize, startPoint.y(), startPoint.x() + x * Global::pixelSize, startPoint.y() + comImg.height * Global::pixelSize);
    }
    for(int y = 0; y < comImg.height; y++)
    {
        painter->drawLine(startPoint.x(), startPoint.y() + y * Global::pixelSize, startPoint.x() + comImg.width * Global::pixelSize, startPoint.y() + y * Global::pixelSize);
    }

    // 外边框
    pen.setColor(Qt::yellow);
    pen.setWidth(2);
    painter->setPen(pen);
    QRectF rect(startPoint.x(), startPoint.y(), comImg.width * Global::pixelSize + 1, comImg.height * Global::pixelSize + 1);
    painter->drawRect(rect);
}

void QGraphicsComImgCanvansItem::paintDragItem(QPainter *painter)
{
    // 在指定位置绘制单个图形
    auto paintItem = ([&](int x0, int y0, QImage img){
        QRect canvasRect(startPoint.x(), startPoint.y(), comImg.width * Global::pixelSize, comImg.height * Global::pixelSize);
        for(int x = 0; x < img.width(); x++)
        {
            for(int y = 0; y < img.height(); y++)
            {
                QColor color = img.pixelColor(x, y);
                quint8 grayscale  = qGray(color.rgb());
                QRect rect(startPoint.x() + (x0 + x) * Global::pixelSize + 1, startPoint.y() + (y0 + y) * Global::pixelSize + 1, Global::pixelSize - 1, Global::pixelSize - 1);
                if(canvasRect.contains(rect.topLeft()))
                    painter->fillRect(rect, grayscale < 128 ? Global::pixelColor_1 : Global::pixelColor_0);
            }
        }
        QPen pen(Global::selectedItemBoundColor);
        pen.setWidth(2);
        painter->setPen(pen);
        QRect rect(startPoint.x() + x0 * Global::pixelSize, startPoint.y() + y0 * Global::pixelSize, Global::pixelSize * img.width(), Global::pixelSize * img.height());
        painter->drawRect(rect);
    });


    if(isDragImg)
    {
        QImage img = rd->getImage(dragImgId);
        paintItem(currentPixel.x() - img.width() / 2, currentPixel.y() - img.height() / 2, img);
    }
}

void QGraphicsComImgCanvansItem::paintAuxiliaryLines(QPainter *painter)
{
    QPen pen(Qt::green);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    auto paintLine = [=](Qt::Orientation dir, int scale){
        if(dir == Qt::Horizontal)
        {
            painter->drawLine(startPoint.x(), startPoint.y() + scale * Global::pixelSize, startPoint.x() + comImg.width * Global::pixelSize, startPoint.y() + scale * Global::pixelSize);
        }
        else
        {
            painter->drawLine(startPoint.x() + scale * Global::pixelSize, startPoint.y(), startPoint.x() + scale * Global::pixelSize, startPoint.y() + comImg.height * Global::pixelSize);
        }
    };

    foreach(auto line, auxiliaryLines)
    {
        paintLine(line.dir, line.scale);
    }
}

QPoint QGraphicsComImgCanvansItem::pointToPixel(QPoint point)
{
    return QPoint((point.x() - startPoint.x()) / Global::pixelSize,
                  (point.y() - startPoint.y()) / Global::pixelSize);
}



int QGraphicsComImgCanvansItem::getPointImgIndex(QPoint point)
{
    int index = -1;
    for(int i = 0; i < comImg.items.size(); i++)
    {
        QImage img = rd->getImage(comImg.items[i].id);
        QRect rect(comImg.items[i].x, comImg.items[i].y, img.width(), img.height());
        if(rect.contains(pointToPixel(point)))
        {
            index = i;
        }
    }
    return index;
}

int QGraphicsComImgCanvansItem::getPointAuxLineIndex(QPoint point)
{
    int index = -1;
    for(int i = 0; i < auxiliaryLines.size(); i++)
    {
        QRect lineRect;
        AuxiliaryLine auxLine = auxiliaryLines[i];
        if(auxLine.dir == Qt::Horizontal)
        {
            lineRect.setRect(startPoint.x(), startPoint.y() + auxLine.scale * Global::pixelSize - 2, comImg.width * Global::pixelSize, 5);
        }
        else
        {
            lineRect.setRect(startPoint.x() + auxLine.scale * Global::pixelSize - 2, startPoint.y(), 5, comImg.height * Global::pixelSize);
        }
        if(lineRect.contains(point))
        {
            index = i;
        }
    }

    return index;
}

void QGraphicsComImgCanvansItem::setComImg(ComImg &comImg)
{
    this->comImg = comImg;
}

void QGraphicsComImgCanvansItem::setRawData(RawData *rd)
{
    this->rd = rd;
    view->viewport()->update();
}

void QGraphicsComImgCanvansItem::deleteSelectItem()
{
    if(selectedItemIndex != -1)
    {
        comImg.items.removeAt(selectedItemIndex);
        selectedItemIndex = -1;
        view->viewport()->update();
    }
}

void QGraphicsComImgCanvansItem::on_MoveUp()
{
    if(selectedItemIndex != -1)
    {
        if(selectedItemIndex + 1 < comImg.items.size())
        {
            comImg.items.swapItemsAt(selectedItemIndex, selectedItemIndex + 1);
            selectedItemIndex++;
            view->viewport()->update();
        }
    }
}

void QGraphicsComImgCanvansItem::on_MoveDown()
{
    if(selectedItemIndex != -1)
    {
        if(selectedItemIndex > 0)
        {
            comImg.items.swapItemsAt(selectedItemIndex, selectedItemIndex - 1);
            selectedItemIndex--;
            view->viewport()->update();
        }
    }
}

void QGraphicsComImgCanvansItem::on_MoveTop()
{
    if(selectedItemIndex != -1)
    {
        while (selectedItemIndex + 1 < comImg.items.size())
        {
            comImg.items.swapItemsAt(selectedItemIndex, selectedItemIndex + 1);
            selectedItemIndex++;
        }
        view->viewport()->update();
    }
}

void QGraphicsComImgCanvansItem::on_MoveBottom()
{
    if(selectedItemIndex != -1)
    {
        while (selectedItemIndex > 0)
        {
            comImg.items.swapItemsAt(selectedItemIndex, selectedItemIndex - 1);
            selectedItemIndex--;
        }
        view->viewport()->update();
    }
}




void QGraphicsComImgCanvansItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->hasFormat("bm/type"))
    {
        // 仅接收图片
        if(event->mimeData()->data("bm/type") == "image")
        {
            isDragImg = true;
            dragImgId = event->mimeData()->data("bm/id").toInt();
            selectedItemIndex = -1;
            event->setAccepted(true);
            return;
        }
    }
    event->setAccepted(false);
}

void QGraphicsComImgCanvansItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    isDragImg = false;
    view->viewport()->update();
}

void QGraphicsComImgCanvansItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    isDragImg = false;
    QImage img = rd->getImage(dragImgId);
    ComImgItem item(currentPixel.x() - img.width() / 2, currentPixel .y() - img.height() / 2, dragImgId);
    comImg.items << item;
}

void QGraphicsComImgCanvansItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    currentPoint = event->pos().toPoint();
    currentPixel = pointToPixel(currentPoint);
    view->viewport()->update();
}

void QGraphicsComImgCanvansItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() << "mouseMoveEvent";
}

void QGraphicsComImgCanvansItem::on_MousePress(QPoint point)
{
    selectedItemIndex = getPointImgIndex(point);
    selectedAuxiliaryLine = getPointAuxLineIndex(point);

    if(action == ActionNull)
    {
        if(selectedAuxiliaryLine != -1)
        {
            action = ActionSelectAuxiliaryLine;
        }
        else if(selectedItemIndex != -1)
        {
            action = ActionSelect;
            moveStartPixel = currentPixel;
        }
    }

    view->viewport()->update();
}

void QGraphicsComImgCanvansItem::on_MouseMove(QPoint point)
{
    auto itemMove([&](){
        if(currentPixel != moveLastPixel)
        {
            // TODO：获取指定ID的图片
            if(selectedItemIndex < comImg.items.size())
            {
                comImg.items[selectedItemIndex].x += currentPixel.x() - moveLastPixel.x();
                comImg.items[selectedItemIndex].y += currentPixel.y() - moveLastPixel.y();
            }
            moveLastPixel = currentPixel;
        }
    });

    auto auxLineMove = [&](){
        if(currentPixel != moveLastPixel)
        {
            AuxiliaryLine *line = &auxiliaryLines[selectedAuxiliaryLine];
            if(line->dir == Qt::Horizontal)
            {
                line->scale += currentPixel.y() - moveLastPixel.y();
            }
            else
            {
                line->scale += currentPixel.x() - moveLastPixel.x();
            }
            moveLastPixel = currentPixel;
        }
    };

    currentPoint = point;
    currentPixel = pointToPixel(point);

    if(action == ActionSelectAuxiliaryLine)
    {
        action = ActionMoveAuxiliaryLine;
        moveLastPixel = currentPixel;
        AuxiliaryLine auxLine = auxiliaryLines.at(selectedAuxiliaryLine);
        view->setCursor(auxLine.dir == Qt::Horizontal ? Qt::SizeVerCursor : Qt::SizeHorCursor);
    }
    else if(action == ActionMoveAuxiliaryLine)
    {
        auxLineMove();
    }
    else if(action == ActionSelect)
    {
        action = ActionMove;
        moveLastPixel = currentPixel;
        view->setCursor(Qt::SizeAllCursor);
    }
    else if(action == ActionMove)
    {
        itemMove();
    }

    emit setStatusBarInfo(currentPixel, QSize(comImg.width, comImg.height));
}

void QGraphicsComImgCanvansItem::on_MouseRelease(QPoint point)
{
    auto removeAuxLine = [&](){
        AuxiliaryLine auxLine = auxiliaryLines.at(selectedAuxiliaryLine);
        if(auxLine.scale < 0)
        {
            auxiliaryLines.remove(selectedAuxiliaryLine);
        }
    };

    if(action == ActionMoveAuxiliaryLine)
    {
        removeAuxLine();
    }


    if(action == ActionSelect || action == ActionMove || action == ActionSelectAuxiliaryLine || action == ActionMoveAuxiliaryLine)
    {
        action = ActionNull;
        view->setCursor(Qt::ArrowCursor);
    }
}

void QGraphicsComImgCanvansItem::on_CreateAuxLine(Qt::Orientation dir)
{
    AuxiliaryLine auxLine(dir, 0);
    auxiliaryLines << auxLine;
    selectedAuxiliaryLine = auxiliaryLines.size() - 1;
    action = ActionMoveAuxiliaryLine;
    moveLastPixel = currentPixel;
    view->setCursor(auxLine.dir == Qt::Horizontal ? Qt::SizeVerCursor : Qt::SizeHorCursor);
}

QGraphicsComImgCanvansItem::QGraphicsComImgCanvansItem(QObject *parent)
    : QObject{parent}
{
    view = static_cast<QWGraphicsView*>(parent);
    // 初始化左上角0点坐标
    startPoint.setX(Global::scaleWidth + Global::scaleOffset);
    startPoint.setY(Global::scaleWidth + Global::scaleOffset);
    comImg.height = 64;
    comImg.width = 128;

    this->setAcceptHoverEvents(true);
    this->setAcceptDrops(true);

    connect(view, SIGNAL(mousePress(QPoint)), this, SLOT(on_MousePress(QPoint)));
    connect(view, SIGNAL(mouseMovePoint(QPoint)), this, SLOT(on_MouseMove(QPoint)));
    connect(view, SIGNAL(mouseRelease(QPoint)), this, SLOT(on_MouseRelease(QPoint)));
}

QRectF QGraphicsComImgCanvansItem::boundingRect() const
{
    return QRectF(startPoint.x(), startPoint.y(), comImg.width * Global::pixelSize, comImg.height * Global::pixelSize);
}

QPainterPath QGraphicsComImgCanvansItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void QGraphicsComImgCanvansItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    paintBackground(painter);
    paintGrid(painter);
    paintItems(painter);
    paintDragItem(painter);
    paintAuxiliaryLines(painter);
}