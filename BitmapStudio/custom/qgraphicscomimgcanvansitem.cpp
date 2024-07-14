#include "qgraphicscomimgcanvansitem.h"
#include "qwgraphicsview.h"
#include "global.h"
#include <QGraphicsSceneDragDropEvent>
#include <QScrollBar>
#include <QMenu>
#include <QMessageBox>
#include "qcustommenu.h"
#include "gui/dialogresize.h"
#include "gui/dialogposition.h"



bool QGraphicsComImgCanvansItem::isInSizeVerArea(QPoint point)
{
    QRect rect(startPoint.x() + comImg.size.width() * Global::pixelSize / 2 - 6, startPoint.y() + comImg.size.height() * Global::pixelSize, 12, 12);
    return rect.contains(point);
}

bool QGraphicsComImgCanvansItem::isInSizeHorArea(QPoint point)
{
    QRect rect(startPoint.x() + comImg.size.width() * Global::pixelSize, startPoint.y() + comImg.size.height() * Global::pixelSize / 2 - 6, 12, 12);
    return rect.contains(point);
}

bool QGraphicsComImgCanvansItem::isInSizeFDiagArea(QPoint point)
{
    QRect rect(startPoint.x() + comImg.size.width() * Global::pixelSize, startPoint.y() + comImg.size.height() * Global::pixelSize, 12, 12);
    return rect.contains(point);
}

void QGraphicsComImgCanvansItem::paintBackground(QPainter *painter)
{
    // 背景使用像素0的颜色填充
    QRect backgroudRect(startPoint.x(), startPoint.y(), comImg.size.width() * Global::pixelSize, comImg.size.height() * Global::pixelSize);
    painter->fillRect(backgroudRect, Global::pixelColor_0);
}

void QGraphicsComImgCanvansItem::paintItems(QPainter *painter)
{
    // 在指定位置绘制单个图形
    auto paintItem = ([=](int x0, int y0, QImage img){
        QRect canvasRect(startPoint.x(), startPoint.y(), comImg.size.width() * Global::pixelSize, comImg.size.height() * Global::pixelSize);

        QRect rect(QPoint(x0, y0), img.size());
        if(rect.right() >= comImg.size.width())
            rect.setRight(comImg.size.width() - 1);
        if(rect.bottom() >= comImg.size.height())
            rect.setBottom(comImg.size.height() - 1);
        if(rect.x() < 0)
            rect.setX(0);
        if(rect.y() < 0)
            rect.setY(0);

        if(rect.x() < comImg.size.width() && rect.y() < comImg.size.height())
        {
            painter->fillRect(QRect(startPoint.x() + rect.x() * Global::pixelSize + 1, startPoint.y() + rect.y() * Global::pixelSize + 1,\
                              Global::pixelSize * rect.width() - 1, Global::pixelSize * rect.height() - 1),\
                              Global::gridColor); // 把画布内的用网格色填充，遮挡下面图形的边框
        }

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
        pen.setColor(selectedItems.contains(index) ? Global::selectedItemBoundColor : Global::itemBoundColor);
        pen.setWidth(2);
        painter->setPen(pen);

        QRect rect(startPoint.x() + x0 * Global::pixelSize, startPoint.y() + y0 * Global::pixelSize, Global::pixelSize * size.width() + 1, Global::pixelSize * size.height() + 1);
        painter->drawRect(rect);

        // 绘制选中图形
        if(selectedItems.contains(index))
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
    for(int x = 0; x < comImg.size.width(); x++)
    {
        painter->drawLine(startPoint.x() + x * Global::pixelSize, startPoint.y(), startPoint.x() + x * Global::pixelSize, startPoint.y() + comImg.size.height() * Global::pixelSize);
    }
    for(int y = 0; y < comImg.size.height(); y++)
    {
        painter->drawLine(startPoint.x(), startPoint.y() + y * Global::pixelSize, startPoint.x() + comImg.size.width() * Global::pixelSize, startPoint.y() + y * Global::pixelSize);
    }

    // 外边框
    pen.setColor(Qt::yellow);
    pen.setWidth(2);
    painter->setPen(pen);
    QRectF rect(startPoint.x(), startPoint.y(), comImg.size.width() * Global::pixelSize + 1, comImg.size.height() * Global::pixelSize + 1);
    painter->drawRect(rect);
}

void QGraphicsComImgCanvansItem::paintDragItem(QPainter *painter)
{
    // 在指定位置绘制单个图形
    auto paintItem = ([&](int x0, int y0, QImage img){
        QRect canvasRect(startPoint.x(), startPoint.y(), comImg.size.width() * Global::pixelSize, comImg.size.height() * Global::pixelSize);
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



void QGraphicsComImgCanvansItem::paintResizePoint(QPainter *painter)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    QBrush brush(Qt::white);
    brush.setStyle(Qt::SolidPattern);
    painter->setBrush(brush);
    painter->drawRect(QRect(startPoint.x() + comImg.size.width() * Global::pixelSize, startPoint.y() + comImg.size.height() * Global::pixelSize, 4, 4));
    painter->drawRect(QRect(startPoint.x() + comImg.size.width() * Global::pixelSize / 2 - 2, startPoint.y() + comImg.size.height() * Global::pixelSize, 4, 4));
    painter->drawRect(QRect(startPoint.x() + comImg.size.width() * Global::pixelSize, startPoint.y() + comImg.size.height() * Global::pixelSize / 2 - 2, 4, 4));

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
            QPoint point(startPoint.x() + comImg.size.width() * Global::pixelSize, currentPoint.y());
            painter->drawRect(QRect(startPoint, calibrate(point)));
        }
        else if(action == ActionResizeHor)
        {
            QPoint point(currentPoint.x(), startPoint.y() + comImg.size.height() * Global::pixelSize);
            painter->drawRect(QRect(startPoint, calibrate(point)));
        }
    }
}

void QGraphicsComImgCanvansItem::paintSelectionBox(QPainter *painter)
{
    if (action == ActionMultiSelect)
    {
        QPen pen(Qt::yellow);
        pen.setWidth(1);
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        QRectF rect(QPoint(startPoint.x() + moveStartPixel.x() * Global::pixelSize, startPoint.y() + moveStartPixel.y() * Global::pixelSize),
                    QPoint(startPoint.x() + currentPixel.x() * Global::pixelSize, startPoint.y() + currentPixel.y() * Global::pixelSize));
        painter->drawRect(rect);
    }
}

void QGraphicsComImgCanvansItem::paintItemInfo(QPainter *painter)
{
    if (selectedItems.size() == 1)
    {
        ComImgItem item = comImg.items[selectedItems[0]];
        QImage img = rd->getImage(item.id);
        QString name = rd->getName(item.id);
        QString text = QString(" %1\n X:%2  Y:%3\n W:%4  H:%5")
                           .arg(name)
                           .arg(item.x)
                           .arg(item.y)
                           .arg(img.width())
                           .arg(img.height());
        QFont font;
        font.setFamily("Microsoft YaHei");    // Set the font family
        font.setPointSize(11);      // Set the font size
        // Set the font on the painter
        painter->setFont(font);
        // 将文本分成多行
        QStringList lines = text.split('\n');

        // 获取每一行的宽度并找出最宽的那一行
        QFontMetrics metrics(font);
        int maxWidth = 0;
        for (const QString &line : lines) {
            int lineWidth = metrics.boundingRect(line).width();
            if (lineWidth > maxWidth) {
                maxWidth = lineWidth;
            }
        }
        int height = metrics.height() * lines.size();

        QPen pen(QColor(240, 240, 240, 220));
        QBrush brush;
        brush.setColor(QColor(240, 240, 240, 220));
        brush.setStyle(Qt::SolidPattern);
        painter->setBrush(brush);
        painter->setPen(pen);
        QSize size = QSize(maxWidth + 8, height);

#if ITEM_INFO_FOLLOW
        QPoint rectStartPoint;
        rectStartPoint.setX(startPoint.x() + item.x * Global::pixelSize);
        rectStartPoint.setY(startPoint.y() + (item.y + img.size().height()) * Global::pixelSize + 6);
        if (rectStartPoint.x() < 25 + view->horizontalScrollBar()->value())
        {
            rectStartPoint.setX(25 + view->horizontalScrollBar()->value());
        }
        if (rectStartPoint.y() < 25 + view->verticalScrollBar()->value())
        {
            rectStartPoint.setY(25 + view->verticalScrollBar()->value());
        }
        int offset = view->verticalScrollBar()->isVisible() ? 25 : 6;
        if (rectStartPoint.x() > view->width() - size.width() - offset + view->horizontalScrollBar()->value())
        {
            rectStartPoint.setX(view->width() - size.width() - offset + view->horizontalScrollBar()->value());
        }

        offset = view->horizontalScrollBar()->isVisible() ? 25 : 6;
        if (rectStartPoint.y() > view->height() - size.height() - offset + view->verticalScrollBar()->value())
        {
            rectStartPoint.setY(view->height() - size.height() - offset + view->verticalScrollBar()->value());
        }

#else
        QPoint rectStartPoint(startPoint.x() + 9, view->height() - 70);
        if (view->horizontalScrollBar()->isVisible())   // 有水平滚动条时上移，防止被滚动条挡住
        {
            rectStartPoint.setY(rectStartPoint.y() - 19);
        }
#endif

        QRect rect(rectStartPoint, size);
        painter->drawRect(rect);
        pen.setColor(Global::gridColor);
        painter->setPen(pen);

        painter->drawText(rect, Qt::AlignLeft, text);
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



void QGraphicsComImgCanvansItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit updateStatusBarPos(QPoint(-1, -1));
}

void QGraphicsComImgCanvansItem::emitUpdatePreview()
{
    // 将img2合并到img1，img2的位置在x,y
    auto merge = [](QImage &img1, QImage &img2, int x, int y) ->void {
        for(int i = x; i < img1.width() && i < x + img2.width(); i++)
        {
            for(int j = y; j < img1.height() && j < y + img2.height(); j++)
            {
                if (i >= 0 && j >= 0)
                    img1.setPixelColor(i, j, img2.pixelColor(i-x, j-y));
            }
        }
    };

    auto comImgToImage = [=](){
        QImage image(comImg.size, QImage::Format_RGB888);
        image.fill(Qt::white);
        foreach(auto item, comImg.items)
        {
            if(rd->getDataMap().contains(item.id))
            {
                merge(image, rd->getDataMap()[item.id].image, item.x, item.y);
            }
        }
        return image;
    };

    emit updatePreview(comImgToImage());
}

void QGraphicsComImgCanvansItem::setComImg(ComImg &comImg)
{
    this->comImg = comImg;
    view->scene()->setSceneRect(QRectF(0, 0, comImg.size.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, comImg.size.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
}

void QGraphicsComImgCanvansItem::setRawData(RawData *rd)
{
    this->rd = rd;
    view->viewport()->update();
}

void QGraphicsComImgCanvansItem::resize(QSize size)
{
    comImg.size = size;
    view->scene()->setSceneRect(QRectF(0, 0, comImg.size.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, comImg.size.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
}

void QGraphicsComImgCanvansItem::setItemPos(QPoint pos)
{
    if (selectedItems.size() == 1)
    {
        ComImgItem *ci = &comImg.items[selectedItems[0]];
        ci->x = pos.x();
        ci->y = pos.y();
        view->viewport()->update();
        emit changed(true);
    }
}



void QGraphicsComImgCanvansItem::on_DeleteSelectItem()
{
    if(selectedItems.size() > 0)
    {
        for (int i = selectedItems.size() - 1; i >= 0; --i)
        {
            comImg.items.removeAt(selectedItems[i]);
            selectedItems.removeAt(i);
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_DeleteAll()
{
    QMessageBox *cleanDialog = new QMessageBox;
    cleanDialog->setText(tr("是否清空画布？"));
    cleanDialog->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    cleanDialog->setDefaultButton(QMessageBox::Yes);
    cleanDialog->setIcon(QMessageBox::Question);
    cleanDialog->setButtonText(QMessageBox::Yes, tr("是"));
    cleanDialog->setButtonText(QMessageBox::No, tr("否"));

    int ret = cleanDialog->exec();
    delete cleanDialog;
    if(ret == QMessageBox::Yes)
    {
        comImg.items.clear();
        selectedItems.clear();
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_Raise()
{
    if(selectedItems.size() == 1)
    {
        if(selectedItems[0] + 1 < comImg.items.size())
        {
            comImg.items.swapItemsAt(selectedItems[0], selectedItems[0] + 1);
            selectedItems[0]++;
            view->viewport()->update();
            emit changed(true);
        }
    }
}

void QGraphicsComImgCanvansItem::on_Lower()
{
    if(selectedItems.size() == 1)
    {
        if(selectedItems[0] > 0)
        {
            comImg.items.swapItemsAt(selectedItems[0], selectedItems[0] - 1);
            selectedItems[0]--;
            view->viewport()->update();
            emit changed(true);
        }
    }
}

void QGraphicsComImgCanvansItem::on_RaiseToTop()
{
    if(selectedItems.size() == 1)
    {
        while (selectedItems[0] + 1 < comImg.items.size())
        {
            comImg.items.swapItemsAt(selectedItems[0], selectedItems[0] + 1);
            selectedItems[0]++;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_LowerToBottom()
{
    if(selectedItems.size() == 1)
    {
        while (selectedItems[0] > 0)
        {
            comImg.items.swapItemsAt(selectedItems[0], selectedItems[0] - 1);
            selectedItems[0]--;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_AlignVCenter()
{
     if(!selectedItems.isEmpty())
    {
        for (int i = 0; i < selectedItems.size(); i++)
        {
            ComImgItem *ci = &comImg.items[selectedItems[i]];
            QImage img = rd->getImage(ci->id);
            ci->y = (comImg.size.height() - img.height()) / 2;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_AlignHCenter()
{
     if(!selectedItems.isEmpty())
    {
        for (int i = 0; i < selectedItems.size(); i++)
        {
            ComImgItem *ci = &comImg.items[selectedItems[i]];
            QImage img = rd->getImage(ci->id);
            ci->x = (comImg.size.width() - img.width()) / 2;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_AlignCenter()
{
     if(!selectedItems.isEmpty())
    {
        for (int i = 0; i < selectedItems.size(); i++)
        {
            ComImgItem *ci = &comImg.items[selectedItems[i]];
            QImage img = rd->getImage(ci->id);
            ci->x = (comImg.size.width() - img.width()) / 2;
            ci->y = (comImg.size.height() - img.height()) / 2;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_MoveUp()
{
    if(!selectedItems.isEmpty())
    {
        for (int i = 0; i < selectedItems.size(); i++)
        {
            ComImgItem *ci = &comImg.items[selectedItems[i]];
            QImage img = rd->getImage(ci->id);
            ci->y--;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_MoveDown()
{
    if(!selectedItems.isEmpty())
    {
        for (int i = 0; i < selectedItems.size(); i++)
        {
            ComImgItem *ci = &comImg.items[selectedItems[i]];
            QImage img = rd->getImage(ci->id);
            ci->y++;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_MoveLeft()
{
    if(!selectedItems.isEmpty())
    {
        for (int i = 0; i < selectedItems.size(); i++)
        {
            ComImgItem *ci = &comImg.items[selectedItems[i]];
            QImage img = rd->getImage(ci->id);
            ci->x--;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_MoveRight()
{
    if(!selectedItems.isEmpty())
    {
        for (int i = 0; i < selectedItems.size(); i++)
        {
            ComImgItem *ci = &comImg.items[selectedItems[i]];
            QImage img = rd->getImage(ci->id);
            ci->x++;
        }
        view->viewport()->update();
        emit changed(true);
    }
}

void QGraphicsComImgCanvansItem::on_SetPos()
{
    // 获取父窗口指针
    QWidget *parentWidget = nullptr;
    if (scene() && scene()->views().size() > 0) {
        parentWidget = scene()->views().at(0)->window();
    }

    if (selectedItems.size() == 1)
    {
        DialogPosition *dlgPosition = new DialogPosition(parentWidget);

        ComImgItem item = comImg.items[selectedItems[0]];
        QPoint pos(item.x, item.y);
        dlgPosition->setDefaultPos(pos);
        int ret = dlgPosition->exec();
        if (ret == QDialog::Accepted)
        {
            QPoint pos = dlgPosition->getPos();
            setItemPos(pos);
        }
        delete dlgPosition;
    }
}

void QGraphicsComImgCanvansItem::on_ResizeCanvas()
{
    // 获取父窗口指针
    QWidget *parentWidget = nullptr;
    if (scene() && scene()->views().size() > 0) {
        parentWidget = scene()->views().at(0)->window();
    }

    DialogResize *dlgResize = new DialogResize(parentWidget);
    QSize defaultSize = getComImg().size;
    dlgResize->setDefaultSize(defaultSize);
    int ret = dlgResize->exec();
    if(ret == QDialog::Accepted)
    {
        QSize size = dlgResize->getSize();
        resize(size);
    }
    delete dlgResize;
}

void QGraphicsComImgCanvansItem::on_OpenImage()
{
    if (selectedItems.size() == 1)
    {
        emit openImgTab(rd->getProject(), comImg.items[selectedItems[0]].id);
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
            selectedItems.clear();
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
    Q_UNUSED(event);
    isDragImg = false;
    QImage img = rd->getImage(dragImgId);
    ComImgItem item(currentPixel.x() - img.width() / 2, currentPixel .y() - img.height() / 2, dragImgId);
    comImg.items << item;
    emit changed(true);
}

void QGraphicsComImgCanvansItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    currentPoint = event->pos().toPoint();
    currentPixel = pointToPixel(currentPoint);
    view->viewport()->update();
}

void QGraphicsComImgCanvansItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    auto itemMove([&](){
        if(currentPixel != moveLastPixel)
        {
            // TODO：获取指定ID的图片
            if (!selectedItems.isEmpty())
            {
                for (int i = 0; i < comImg.items.size(); i++)
                {
                    if (selectedItems.contains(i))
                    {
                        comImg.items[i].x += currentPixel.x() - moveLastPixel.x();
                        comImg.items[i].y += currentPixel.y() - moveLastPixel.y();
                    }
                }
                view->setCursor(Qt::SizeAllCursor);
            }

            moveLastPixel = currentPixel;
        }
    });


    if(action == ActionNull)
    {
        if (getPointImgIndex(currentPoint) == -1)
        {
            action = ActionMultiSelect;
            moveStartPixel = currentPixel;
        }
        else
        {
            Qt::CursorShape cursor = Qt::ArrowCursor;
            if(isInSizeFDiagArea(currentPoint))
            {
                cursor = Qt::SizeFDiagCursor;
            }
            else if(isInSizeVerArea(currentPoint))
            {
                cursor = Qt::SizeVerCursor;
            }
            else if(isInSizeHorArea(currentPoint))
            {
                cursor = Qt::SizeHorCursor;
            }
            view->setCursor(cursor);
        }
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
    else if(action == ActionResizeFDiag)
    {
        newSize = QSize(currentPixel.x(), currentPixel.y());
        emit updateStatusBarSize(newSize);
    }
    else if(action == ActionResizeVer)
    {
        newSize = QSize(comImg.size.width(), currentPixel.y());
        emit updateStatusBarSize(newSize);
    }
    else if(action == ActionResizeHor)
    {
        newSize = QSize(currentPixel.x(), comImg.size.height());
        emit updateStatusBarSize(newSize);
    }
    else if (action == ActionMultiSelect)
    {

    }

    emit updateStatusBarPos(currentPixel);
}

void QGraphicsComImgCanvansItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint point = event->pos().toPoint();

    auto leftPressEvent = [=](){

        int index = getPointImgIndex(event->pos().toPoint());

        if (index != -1)
        {
            if (!selectedItems.contains(index))
            {
                selectedItems = {index};
            }
        }
        else
        {
            selectedItems.clear();
        }

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
            else if(!selectedItems.isEmpty())
            {
                action = ActionSelect;
                moveStartPixel = currentPixel;
            }
        }
    };

    auto rightPressEvent = [=]() {
        int index = getPointImgIndex(event->pos().toPoint());

        if (index != -1)
        {
            if (!selectedItems.contains(index))
            {
                selectedItems = {index};
            }
        }
        else
        {
            selectedItems.clear();
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

    view->viewport()->update();
}

void QGraphicsComImgCanvansItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    auto updateSelectedItems = [&]() {
        selectedItems.clear();
        for (int i = 0; i < comImg.items.size(); i++)
        {
            auto item = comImg.items[i];
            QImage img = rd->getImage(item.id);
            QRect selectRect(moveStartPixel, currentPixel);
            if (selectRect.contains(QRect(QPoint(item.x, item.y), img.size())))
            {
                selectedItems << i;
            }
        }
    };


    if(action == ActionResizeFDiag || action == ActionResizeVer || action == ActionResizeHor)
    {
        action = ActionNull;
        comImg.size = newSize;
        view->scene()->setSceneRect(QRectF(0, 0, comImg.size.width() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset, comImg.size.height() * Global::pixelSize + Global::scaleWidth + Global::scaleOffset));
    }
    else if(action == ActionSelect || action == ActionMove)
    {
        if(action == ActionMove)
        {
            emit changed(true);
        }
        action = ActionNull;
        view->setCursor(Qt::ArrowCursor);
    }
    else if (action == ActionMultiSelect)
    {
        updateSelectedItems();
        action = ActionNull;
    }

    view->viewport()->update();

    emitUpdatePreview();
}

void QGraphicsComImgCanvansItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    auto createAction = [=](QMenu *menu, QString name, QString key, void (QGraphicsComImgCanvansItem::*method)()) {
        QAction *action = menu->addAction(name);
        action->setShortcut(QKeySequence(key)); // 快捷键仅显示作用，实际在formcomingedit里的快捷键有效
        connect(action, &QAction::triggered, this, method);
    };

    QMenu menu;
    int itemIndex = getPointImgIndex(event->pos().toPoint());

    if (itemIndex != -1)    // 选中图片
    {
        if (selectedItems.size() == 1)
        {
            createAction(&menu, tr("打开图片"), "Ctrl+Shift+O", &QGraphicsComImgCanvansItem::on_OpenImage);
            createAction(&menu, tr("设置位置"), "Ctrl+Shift+P", &QGraphicsComImgCanvansItem::on_SetPos);
        }

        QCustomMenu *menuMove = new QCustomMenu(tr("移动"));
        createAction(menuMove, tr("上移"), "Up", &QGraphicsComImgCanvansItem::on_MoveUp);
        createAction(menuMove, tr("下移"), "Down", &QGraphicsComImgCanvansItem::on_MoveDown);
        createAction(menuMove, tr("左移"), "Left", &QGraphicsComImgCanvansItem::on_MoveLeft);
        createAction(menuMove, tr("右移"), "Right", &QGraphicsComImgCanvansItem::on_MoveRight);
        menu.addMenu(menuMove);

        QMenu *menuLayer = new QMenu(tr("图层"));
        createAction(menuLayer, tr("移动到顶层"), "Home", &QGraphicsComImgCanvansItem::on_RaiseToTop);
        createAction(menuLayer, tr("移动到底层"), "End", &QGraphicsComImgCanvansItem::on_LowerToBottom);
        createAction(menuLayer, tr("上移一层"), "PgUp", &QGraphicsComImgCanvansItem::on_Raise);
        createAction(menuLayer, tr("下移一层"), "PgDown", &QGraphicsComImgCanvansItem::on_Lower);
        if (selectedItems.size() == 1)
            menu.addMenu(menuLayer);

        QMenu *menuAlign = new QMenu(tr("对齐"));
        createAction(menuAlign, tr("水平对齐"), "Ctrl+Alt+H", &QGraphicsComImgCanvansItem::on_AlignHCenter);
        createAction(menuAlign, tr("垂直对齐"), "Ctrl+Alt+V", &QGraphicsComImgCanvansItem::on_AlignVCenter);
        createAction(menuAlign, tr("中心对齐"), "Ctrl+Alt+C", &QGraphicsComImgCanvansItem::on_AlignCenter);
        menu.addMenu(menuAlign);

        createAction(&menu, tr("删除"), "Delete", &QGraphicsComImgCanvansItem::on_DeleteSelectItem);
    }
    else    // 其余空白区域
    {
        createAction(&menu, tr("调整画布大小"), "Ctrl+Shift+R", &QGraphicsComImgCanvansItem::on_ResizeCanvas);
        createAction(&menu, tr("清空画布"), "Ctrl+Shift+X", &QGraphicsComImgCanvansItem::on_DeleteAll);
    }





    // 在鼠标右键点击的位置显示菜单
    menu.exec(event->screenPos());

}

void QGraphicsComImgCanvansItem::on_MouseMove(QPoint point)
{
    currentPoint = point;
    currentPixel = pointToPixel(point);

    if(action == ActionNull)
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
}


QGraphicsComImgCanvansItem::QGraphicsComImgCanvansItem(QObject *parent)

{
    view = static_cast<QWGraphicsView*>(parent);
    // 初始化左上角0点坐标
    startPoint.setX(Global::scaleWidth + Global::scaleOffset);
    startPoint.setY(Global::scaleWidth + Global::scaleOffset);
    comImg.size = QSize(128, 64);

    // 获取mainwindow指针
    auto getMainWindow = [=]() {
        QObject *obj = parent;
        while (1)
        {
            if (obj->objectName() == "MainWindow")
            {
                return obj;
            }
            obj = obj->parent();
        }
    };
    connect(this, SIGNAL(openImgTab(QString, int)), getMainWindow(), SLOT(on_OpenImgTab(QString, int)));
    connect(view, SIGNAL(mouseMovePoint(QPoint)), this, SLOT(on_MouseMove(QPoint)));
}

QRectF QGraphicsComImgCanvansItem::boundingRect() const
{
    return QRectF(startPoint.x(), startPoint.y(), comImg.size.width() * Global::pixelSize, comImg.size.height() * Global::pixelSize);
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
    paintSelectionBox(painter);
    paintResizePoint(painter);
    paintItemInfo(painter);
    emitUpdatePreview();
}
