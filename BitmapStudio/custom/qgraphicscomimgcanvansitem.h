#ifndef QGRAPHICSCOMIMGCANVANSITEM_H
#define QGRAPHICSCOMIMGCANVANSITEM_H

#include <custom/qgraphicsitembase.h>
#include "core/rawdata.h"

class QGraphicsComImgCanvansItem : public QGraphicsItemBase
{
    Q_OBJECT

    enum Action
    {
        ActionNull,
        ActionSelect,
        ActionMove,
        ActionSelectAuxiliaryLine,
        ActionMoveAuxiliaryLine,
        ActionResizeVer,
        ActionResizeHor,
        ActionResizeFDiag,
    };

    // https://blog.csdn.net/u013125105/article/details/100514290
    // Drag Drop参考

    QGraphicsView *view;
    QPoint startPoint;  // 左上角起始位置坐标
    ComImg comImg;
    RawData *rd = nullptr;

    Action action = ActionNull;
    QSize newSize;
    int selectedItemIndex = -1;
    QPoint currentPoint;
    QPoint currentPixel;    // 当前鼠标所在的坐标（图片像素）
    QPoint moveLastPixel;   // 移动图元上一次所在的坐标(像素)
    QPoint moveStartPixel;  // 开始移动画布的坐标(像素)
    int dragImgId;          // 拖入的图片ID
    bool isDragImg = false;

    
    bool isInSizeVerArea(QPoint point); // 是否处于垂直调整画布大小的区域内
    bool isInSizeHorArea(QPoint point); // 是否处于水平调整画布大小的区域内
    bool isInSizeFDiagArea(QPoint point);
    void paintBackground(QPainter *painter);    // 绘制画布
    void paintItems(QPainter *painter);        // 绘制图形元素
    void paintGrid(QPainter *painter);          // 绘制网格
    void paintDragItem(QPainter *painter);      // 绘制拖入的图片
    void paintAuxiliaryLines(QPainter *painter);    // 绘制辅助线
    void paintResizePoint(QPainter *painter);  // 绘制调整画布大小的点
    void paintItemInfo(QPainter *painter);      // 绘制选择的图形的名称、位置等信息
    QPoint pointToPixel(QPoint point);  // 坐标转换为画布上的像素坐标
    int getPointImgIndex(QPoint point);
    int getPointAuxLineIndex(QPoint point);

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void emitUpdatePreview();   // 更新预览显示显示

public:
    explicit QGraphicsComImgCanvansItem(QObject *parent = nullptr);
    ~QGraphicsComImgCanvansItem() { rd = nullptr; }
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;

    void setComImg(ComImg &comImg);
    void setRawData(RawData *rd);
    ComImg getComImg() { return comImg; }
    void resize(QSize size);
    void setItemPos(QPoint);
    QPoint getSelectedItemPos();
    ComImgItem getSelectedItem();
    QString getProject() { return rd->getProject(); }

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;//处理drop
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;//添加QDrag
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
public slots:
    void on_MousePress(QPoint point);
    void on_MouseMove(QPoint point);
    void on_MouseRelease(QPoint point);
    void on_CreateAuxLine(Qt::Orientation dir);

    void deleteSelectItem();    // 删除选中的图片
    void deleteAll();           // 删除所有
    void on_Forward();           // 上移一层
    void on_Backward();         // 下移一层
    void on_Top();
    void on_Bottom();
    void on_AlignVCenter();
    void on_AlignHCenter();
    void on_AlignCenter();
    void on_MoveUp();
    void on_MoveDown();
    void on_MoveLeft();
    void on_MoveRight();


signals:
    void updateStatusBarPos(QPoint);
    void updateStatusBarSize(QSize);
    void updatePreview(QImage);
    void changed(bool);
};

#endif // QGRAPHICSCOMIMGCANVANSITEM_H
