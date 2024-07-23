#ifndef QGRAPHICSCANVASITEM_H
#define QGRAPHICSCANVASITEM_H

#include "custom/qgraphicsitembase.h"
#include <QCursor>


class QGraphicsCanvasItem : public QGraphicsItemBase
{
    Q_OBJECT
private:
    enum Action
    {
        ActionNull,
        ActionResizeVer,
        ActionResizeHor,
        ActionResizeFDiag,
        ActionMove,
        ActionWrite,
        ActionErase,
    };

    QGraphicsView *view;
    QImage image;
    QPoint startPoint;  // 左上角起始位置坐标
    QPoint currentPoint;
    QPoint currentPixel;    // 当前鼠标所在的坐标（图片像素）
    QPoint moveStartPixel;  // 开始移动画布的坐标(像素)
    QPoint moveLastPixel;   // 移动图元上一次所在的坐标(像素)
    QSize newSize;          // 调整画布大小的新的大小
    QCursor cursorPencil;
    QCursor cursorEraser;

    QPoint pointToPixel(QPoint point);  // 坐标转换为画布上的像素坐标
    bool isInSizeVerArea(QPoint point); // 是否处于垂直调整画布大小的区域内
    bool isInSizeHorArea(QPoint point); // 是否处于水平调整画布大小的区域内
    bool isInSizeFDiagArea(QPoint point);
    bool isInImgArea(QPoint point);     // 是否在图片的区域内
    quint8 action = ActionNull;  // 调整画布大小的步骤 0-初始状态 1-按下
    void resizeImage(QImage &img, QSize size);
    void moveImage(QImage &img,  int OffsetX, int OffsetY);
    void reserveImage(QImage &img);
    void flipHor(QImage &img);
    void flipVer(QImage &img);
    void rotateLeft(QImage &img);
    void rotateRight(QImage &img);
    void getMargin(int &up, int &down, int &left, int &right);  // 获取四周的距离
    void drawPoint(QImage &img, QPoint point, bool dot);
    void drawLine(QImage &img, QPoint point1, QPoint point2, bool dot);


public:
    enum Mode
    {
        SelectMode,
        EditMode,
        MeasureMode,
    };

    explicit QGraphicsCanvasItem(QWidget *parent = nullptr);
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;
    void setImage(QImage &image);
    QImage getImage();
    void resize(QSize size);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
public slots:
    void on_MouseMove(QPoint point);
    void on_EditModeChanged();
    void on_Reserve();  // 反色
    void on_AlignCenter();   // 居中
    void on_AlignHCenter(); // 水平居中
    void on_AlignVCenter(); // 垂直居中
    void on_AutoResize();   // 自动裁剪
    void on_MoveUp();
    void on_MoveDown();
    void on_MoveLeft();
    void on_MoveRight();
    void on_FlipHor();
    void on_FlipVer();
    void on_RotateLeft();
    void on_RotateRight();

signals:
    void updateStatusBarPos(QPoint);
    void updateStatusBarSize(QSize);
    void updateStatusBarMode(QString);
    void updatePreview(QImage);
    void changed(bool);
};

#endif // QGRAPHICSCANVASITEM_H
