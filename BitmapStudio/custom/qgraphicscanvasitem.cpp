#include "qgraphicscanvasitem.h"
#include "qwgraphicsview.h"
#include "global.h"
#include <QDebug>
#include <QPen>
#include <QImage>
#include <QMenu>
#include <QScrollBar>
#include <QApplication>
#include <QGraphicsSceneContextMenuEvent>
#include <QtConcurrent/QtConcurrent>

QPoint QGraphicsCanvasItem::pointToPixel(QPoint point)
{
    return QPoint((point.x() - startPoint.x()) / Global::pixelSize,
                  (point.y() - startPoint.y()) / Global::pixelSize);
}

bool QGraphicsCanvasItem::isInSizeVerArea(QPoint point)
{
    QRect rect(startPoint.x() + image.width() * Global::pixelSize / 2 - 6, startPoint.y() + image.height() * Global::pixelSize, 12, 12);
    return rect.contains(point);
}

bool QGraphicsCanvasItem::isInSizeHorArea(QPoint point)
{
    QRect rect(startPoint.x() + image.width() * Global::pixelSize, startPoint.y() + image.height() * Global::pixelSize / 2 - 6, 12, 12);
    return rect.contains(point);
}

bool QGraphicsCanvasItem::isInSizeFDiagArea(QPoint point)
{
    QRect rect(startPoint.x() + image.width() * Global::pixelSize, startPoint.y() + image.height() * Global::pixelSize, 12, 12);
    return rect.contains(point);
}

bool QGraphicsCanvasItem::isInImgArea(QPoint point)
{
    QRectF rect(startPoint.x(), startPoint.y(), image.width() * Global::pixelSize, image.height() * Global::pixelSize);
    return rect.contains(point);
}

void QGraphicsCanvasItem::resizeImage(QImage &img, QSize size)
{
    QImage newImage(size.width(), size.height(), QImage::Format_RGB888);
    newImage.fill(Qt::white);

    for(int x = 0; x < img.width() && x < newImage.width(); x++)
    {
        for(int y = 0; y < img.height() && y < newImage.height(); y++)
        {
            QPoint point(x, y);
            newImage.setPixelColor(point, image.pixelColor(point));
        }
    }

    img = newImage;
    emit changed(true);
}

void QGraphicsCanvasItem::moveImage(QImage &img, int OffsetX, int OffsetY)
{
    QImage newImg(img.size(), QImage::Format_RGB888);
    newImg.fill(Qt::white);

    auto isContainPoint([=](QPoint point){
        return (point.x() >= 0 && point.x() < img.width() && point.y() >= 0 && point.y() < img.height());
    });

    for(int x = 0; x < newImg.width(); x++)
    {
        for(int y = 0; y < newImg.height(); y++)
        {
            QPoint point(x + OffsetX, y + OffsetY);
            if(isContainPoint(point))
            {
                newImg.setPixelColor(point, img.pixelColor(x, y));
            }
        }
    }
    img = newImg;
    emit changed(true);
}

void QGraphicsCanvasItem::reserveImage(QImage &img)
{
    QImage newImg(img.size(), QImage::Format_RGB888);
    for(int x = 0; x < img.width(); x++)
    {
        for(int y = 0; y < img.height(); y++)
        {
            QColor color = image.pixelColor(x, y);
            color.setRgb(0xff - color.red(), 0xff - color.green(), 0xff - color.blue());
            newImg.setPixelColor(x, y, color);
        }
    }
    img = newImg;
    emit changed(true);
}

void QGraphicsCanvasItem::flipHor(QImage &img)
{
    QImage newImg(img.size(), QImage::Format_RGB888);
    for(int x = 0; x < img.width(); x++)
    {
        for(int y = 0; y < img.height(); y++)
        {
            newImg.setPixelColor(img.width() - 1 - x, y, img.pixelColor(x, y));
        }
    }
    img = newImg;
    emit changed(true);
}

void QGraphicsCanvasItem::flipVer(QImage &img)
{
    QImage newImg(img.size(), QImage::Format_RGB888);
    for(int x = 0; x < img.width(); x++)
    {
        for(int y = 0; y < img.height(); y++)
        {
            newImg.setPixelColor(x, img.height() - 1 - y, img.pixelColor(x, y));
        }
    }
    img = newImg;
    emit changed(true);
}

void QGraphicsCanvasItem::rotateLeft(QImage &img)
{
    QImage newImg(img.height(), img.width(), QImage::Format_RGB888);
    for(int x = 0; x < img.width(); x++)
    {
        for(int y = 0; y < img.height(); y++)
        {
            newImg.setPixelColor(y, img.width() - 1 - x, img.pixelColor(x, y));
        }
    }
    img = newImg;
    emit changed(true);
}

void QGraphicsCanvasItem::rotateRight(QImage &img)
{
    QImage newImg(img.height(), img.width(), QImage::Format_RGB888);
    for(int x = 0; x < img.width(); x++)
    {
        for(int y = 0; y < img.height(); y++)
        {
            newImg.setPixelColor(img.height() - 1 - y, x, img.pixelColor(x, y));
        }
    }
    img = newImg;
    emit changed(true);
}

void QGraphicsCanvasItem::getMargin(int &up, int &down, int &left, int &right)
{
    auto isEmptyRow = ([=](int row){
        for(int i = 0; i < image.width(); i++)
        {
            if(qGray(image.pixelColor(i, row).rgb()) < 128)
                return false;
        }
        return true;
    });

    auto isEmptyCol = ([=](int col){
        for(int i = 0; i < image.height(); i++)
        {

            if(qGray(image.pixelColor(col, i).rgb()) < 128)
                return false;
        }
        return true;
    });

    for(int i = 0; i < image.height(); i++)
    {
        if(!isEmptyRow(i))
        {
            up = i;
            break;
        }
        // 全部为空，则直接退出
        if(i == image.height() - 1)
        {
            return;
        }
    }

    for(int i = 0; i < image.height(); i++)
    {
        if(!isEmptyRow(image.height() - 1 -i))
        {
            down = i;
            break;
        }
    }

    for(int i = 0; i < image.width(); i++)
    {
        if(!isEmptyCol(i))
        {
            left = i;
            break;
        }
    }

    for(int i = 0; i < image.width(); i++)
    {
        if(!isEmptyCol(image.width() - 1 - i))
        {
            right = i;
            break;
        }
    }
}



void QGraphicsCanvasItem::drawPoint(QImage &img, QPoint point, bool dot)
{
    if(point.x() < img.width() && point.y() < img.height())
    {
        img.setPixelColor(point, QColor(dot ? Qt::black : Qt::white));
        emit changed(true);
    }
}

void QGraphicsCanvasItem::drawLine(QImage &img, QPoint point1, QPoint point2, bool dot)
{
    int x1 = point1.x();
    int y1 = point1.y();
    int x2 = point2.x();
    int y2 = point2.y();

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;

    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (true) {
        if(x1 < img.width() && y1 < img.height())
        {
            img.setPixelColor(QPoint(x1, y1), QColor(dot ? Qt::black : Qt::white));
        }

        if (x1 == x2 && y1 == y2) {
            break;
        }

        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y1 += sy;
        }
    }
    emit changed(true);
}





QGraphicsCanvasItem::QGraphicsCanvasItem(QWidget *parent)
{
    view = static_cast<QWGraphicsView*>(parent);
    // 初始化左上角0点坐标
    startPoint.setX(Global::scaleWidth + Global::scaleOffset);
    startPoint.setY(Global::scaleWidth + Global::scaleOffset);
    action = ActionNull;

    connect(view, SIGNAL(mouseMovePoint(QPoint)), this, SLOT(on_MouseMove(QPoint)));

    QPixmap cursorPencilPixmap(":/Image/Cursor/Pencil.svg");
    cursorPencil = QCursor(cursorPencilPixmap, 0, 0);

    QPixmap cursorEraserPixmap(":/Image/Cursor/Eraser.svg");
    cursorEraser = QCursor(cursorEraserPixmap, 0, 0);
}

QRectF QGraphicsCanvasItem::boundingRect() const
{
    QRectF rect(startPoint.x(), startPoint.y(), image.width() * Global::pixelSize, image.height() * Global::pixelSize);
    return rect;
}

QPainterPath QGraphicsCanvasItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void QGraphicsCanvasItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(image.isNull())
        return;


    // 绘制像素
    QImage imageShow = image;
    if(action == ActionMove)   // 如果是移动画布，对图片位置进行移动
    {
        moveImage(imageShow, currentPixel.x() - moveStartPixel.x(), currentPixel.y() - moveStartPixel.y());
    }

    for(int x = 0; x < imageShow.width(); x++)
    {
        for(int y = 0; y < imageShow.height(); y++)
        {
            QColor color = imageShow.pixelColor(x, y);
            quint8 grayscale  = qGray(color.rgb());
            QRect rect(startPoint.x() + x * Global::pixelSize, startPoint.y() + y * Global::pixelSize, Global::pixelSize, Global::pixelSize);
            painter->fillRect(rect, grayscale < 128 ? Global::pixelColor_1 : Global::pixelColor_0);
        }
    }

    QElapsedTimer timer;
    timer.start();


    // 绘制网格
    QPen pen(Global::gridColor);
    painter->setPen(pen);
    for(int x = 0; x < image.width(); x++)
    {
        painter->drawLine(startPoint.x() + x * Global::pixelSize, startPoint.y(), startPoint.x() + x * Global::pixelSize, startPoint.y() + image.height() * Global::pixelSize);
    }
    for(int y = 0; y < image.height(); y++)
    {
        painter->drawLine(startPoint.x(), startPoint.y() + y * Global::pixelSize, startPoint.x() + image.width() * Global::pixelSize, startPoint.y() + y * Global::pixelSize);
    }
    // qint64 elapsedTime = timer.nsecsElapsed();
    // qDebug() << "drawPixel function took" << elapsedTime << "nanoseconds";
    // 外边框
    pen.setWidth(2);
    pen.setColor(Qt::yellow);
    painter->setPen(pen);
    QRectF rect(startPoint.x(), startPoint.y(), image.width() * Global::pixelSize + 1, image.height() * Global::pixelSize + 1);
    painter->drawRect(rect);

    // 调整画布大小
    //if(isResezeMode)
    {
        pen.setWidth(1);
        pen.setColor(Qt::black);
        painter->setPen(pen);
        QBrush brush(Qt::white);
        brush.setStyle(Qt::SolidPattern);
        painter->setBrush(brush);
        painter->drawRect(QRect(startPoint.x() + image.width() * Global::pixelSize, startPoint.y() + image.height() * Global::pixelSize, 4, 4));
        painter->drawRect(QRect(startPoint.x() + image.width() * Global::pixelSize / 2 - 2, startPoint.y() + image.height() * Global::pixelSize, 4, 4));
        painter->drawRect(QRect(startPoint.x() + image.width() * Global::pixelSize, startPoint.y() + image.height() * Global::pixelSize / 2 - 2, 4, 4));
    }

    // 画布预览

    // 校准画布大小到像素点对应的大小
    auto calibrate = ([=](QPoint point){
        return QPoint(((point.x() - startPoint.x()) / Global::pixelSize) * Global::pixelSize + startPoint.x(),
                      ((point.y() - startPoint.y()) / Global::pixelSize) * Global::pixelSize + startPoint.y());
    });

    if(action != ActionNull)
    {
        QBrush brush;
        brush.setStyle(Qt::NoBrush);
        painter->setBrush(brush);
        pen.setWidth(2);
        pen.setStyle(Qt::DotLine);
        pen.setColor(Qt::yellow);
        painter->setPen(pen);


        if(action == ActionResizeFDiag)
        {
            painter->drawRect(QRect(startPoint, calibrate(currentPoint)));
        }
        else if(action == ActionResizeVer)
        {
            QPoint point(startPoint.x() + image.width() * Global::pixelSize, currentPoint.y());
            painter->drawRect(QRect(startPoint, calibrate(point)));
        }
        else if(action == ActionResizeHor)
        {
            QPoint point(currentPoint.x(), startPoint.y() + image.height() * Global::pixelSize);
            painter->drawRect(QRect(startPoint, calibrate(point)));
        }
    }

    emit updatePreview(image);
}

void QGraphicsCanvasItem::setImage(QImage &image)
{
    if(image.isNull())
    {
        //return;
    }
    // 虽然是单色的，转为为RGB888，像素处理的时候方便一点
    this->image = image.convertToFormat(QImage::Format_RGB888);
    view->scene()->setSceneRect(QRectF(0, 0, image.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, image.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
    view->viewport()->update();
}

QImage QGraphicsCanvasItem::getImage()
{
    return image;
}


void QGraphicsCanvasItem::resize(QSize size)
{
    resizeImage(image, size);
    view->scene()->setSceneRect(QRectF(0, 0, image.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, image.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
}

void QGraphicsCanvasItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint point = event->pos().toPoint();

    auto leftPressEvent = [=]() {
        if(action == ActionNull)
        {
            if(isInSizeFDiagArea(point))
            {
                action = ActionResizeFDiag;
            }
            else if(isInSizeVerArea(point))
            {
                action = ActionResizeVer;
            }
            else if(isInSizeHorArea(point))
            {
                action = ActionResizeHor;
            }
            else
            {
                if(Global::editMode)
                {
                    action = ActionWrite;
                    drawPoint(image, pointToPixel(point), true);
                }
            }
        }
    };

    auto rightPressEvent = [=]() {
        if(Global::editMode)
        {
            action = ActionErase;
            drawPoint(image, pointToPixel(point), false);
        }
    };

    auto middlePressEvent = [=]() {
        if(isInImgArea(point))
        {
            action = ActionMove;
            moveStartPixel = pointToPixel(point);
            view->setCursor(Qt::SizeAllCursor);
        }
    };

    if (event->button() == Qt::LeftButton)
    {
        leftPressEvent();
    }
    else if (event->button() == Qt::RightButton)
    {
        rightPressEvent();
    }
    else if (event->button() == Qt::MiddleButton)
    {
        middlePressEvent();
    }

    view->viewport()->update();
}

void QGraphicsCanvasItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint point = pointToPixel(event->pos().toPoint());
    QPoint lastPoint = pointToPixel(event->lastPos().toPoint());
    if(action == ActionWrite)
    {
        if (lastPoint == point)
        {
            drawPoint(image, point, true);
        }
        else
        {
            drawLine(image, lastPoint, point, true);
        }
    }
    else if(action == ActionErase)
    {
        if (lastPoint == point)
        {
            drawPoint(image, point, false);
        }
        else
        {
            drawLine(image, lastPoint, point, false);
        }
    }
    else if(action == ActionResizeFDiag)
    {
        newSize = QSize(currentPixel.x(), currentPixel.y());
        emit updateStatusBarSize(newSize);
    }
    else if(action == ActionResizeVer)
    {
        newSize = QSize(image.size().width(), currentPixel.y());
        emit updateStatusBarSize(newSize);
    }
    else if(action == ActionResizeHor)
    {
        newSize = QSize(currentPixel.x(), image.size().height());
        emit updateStatusBarSize(newSize);
    }
}

void QGraphicsCanvasItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    if(action != ActionNull)
    {
        if(action == ActionResizeFDiag || action == ActionResizeVer || action == ActionResizeHor)
        {
            resizeImage(image, newSize);
            view->scene()->setSceneRect(QRectF(0, 0, image.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, image.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
            view->setCursor(Qt::ArrowCursor);
        }
        else if(action == ActionMove)
        {
            moveImage(image, currentPixel.x() - moveStartPixel.x(), currentPixel.y() - moveStartPixel.y());
            view->setCursor(Qt::ArrowCursor);
        }

        action = ActionNull;
    }
    view->viewport()->update();
}



void QGraphicsCanvasItem::on_MouseMove(QPoint point)
{
    currentPoint = point;
    currentPixel.setX((currentPoint.x() - startPoint.x()) / Global::pixelSize);
    currentPixel.setY((currentPoint.y() - startPoint.y()) / Global::pixelSize);

    qDebug() << action;
    if (Global::editMode)
    {
        if (action == ActionErase)
        {
            view->setCursor(cursorEraser);
            qDebug() << "eraser";
        }
        else
        {
            view->setCursor(cursorPencil);
            qDebug() << "pencil";
        }
    }
    else if(action == ActionNull)
    {
        Qt::CursorShape cursor = Qt::ArrowCursor;
        if(isInSizeFDiagArea(point))
        {
            cursor = Qt::SizeFDiagCursor;
        }
        else if(isInSizeVerArea(point))
        {
            cursor = Qt::SizeVerCursor;
        }
        else if(isInSizeHorArea(point))
        {
            cursor = Qt::SizeHorCursor;
        }
        view->setCursor(cursor);
    }


    emit updateStatusBarPos(currentPixel);
}



void QGraphicsCanvasItem::on_Reserve()
{
    reserveImage(image);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_AlignCenter()
{
    int upMargin, downMargin, leftMargin, rightMargin;  // 图片离画布边缘的距离
    getMargin(upMargin, downMargin, leftMargin, rightMargin);
    moveImage(image, (leftMargin + rightMargin) / 2 - leftMargin, (upMargin + downMargin) / 2 - upMargin);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_AlignHCenter()
{
    int upMargin, downMargin, leftMargin, rightMargin;  // 图片离画布边缘的距离
    getMargin(upMargin, downMargin, leftMargin, rightMargin);
    moveImage(image, (leftMargin + rightMargin) / 2 - leftMargin, 0);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_AlignVCenter()
{
    int upMargin, downMargin, leftMargin, rightMargin;  // 图片离画布边缘的距离
    getMargin(upMargin, downMargin, leftMargin, rightMargin);
    moveImage(image, 0, (upMargin + downMargin) / 2 - upMargin);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_AutoResize()
{
    int upMargin, downMargin, leftMargin, rightMargin;  // 图片离画布边缘的距离
    getMargin(upMargin, downMargin, leftMargin, rightMargin);
    moveImage(image, -leftMargin, -upMargin);   // 图形移到左上角
    resizeImage(image, QSize(image.width() - leftMargin - rightMargin, image.height() - upMargin - downMargin));
    view->scene()->setSceneRect(QRectF(0, 0, image.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, image.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_MoveUp()
{
    moveImage(image, 0, -1);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_MoveDown()
{
    moveImage(image, 0, 1);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_MoveLeft()
{
    moveImage(image, -1, 0);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_MoveRight()
{
    moveImage(image, 1, 0);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_FlipHor()
{
    flipHor(image);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_FlipVer()
{
    flipVer(image);
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_RotateLeft()
{
    rotateLeft(image);
    view->scene()->setSceneRect(QRectF(0, 0, image.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, image.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
    view->viewport()->update();
}

void QGraphicsCanvasItem::on_RotateRight()
{
    rotateRight(image);
    view->scene()->setSceneRect(QRectF(0, 0, image.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, image.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
    view->viewport()->update();
}



