#ifndef QGRAPHICSSCALEITEM_H
#define QGRAPHICSSCALEITEM_H

#include <QGraphicsItem>
#include <QObject>

class QGraphicsScaleItem : public QGraphicsObject
{
    Q_OBJECT
private:
    struct AuxiliaryLine
    {
        Qt::Orientation dir;
        int scale;
        static bool lock;  // 锁定辅助线
        static bool hide;  // 隐藏辅助线
        AuxiliaryLine(Qt::Orientation dir, int scale)
        {
            this->dir = dir;
            this->scale = scale;
        }
    };

    QGraphicsView *view;
    static QVector<AuxiliaryLine> auxiliaryLines;
    int selectedAuxiliaryLine = -1;     // 选中的辅助线
    QPoint startPoint;      // 画布左上角起始坐标
    QPointF mousePos;
    void drawScale(QPainter *painter);
    void drawAuxiliaryLines(QPainter *painter);
    bool createFlag = false;
    bool isSelected = false;    // 是否选中辅助线
    bool isScaleArea(QPoint point);     // 是否在刻度尺区域
    bool isHorizontalScale(QPoint point);   // 是否在水平刻度线
    bool isVerticalScale(QPoint point);     // 是否在垂直的刻度线区域内
    void sendEventToOtherItems(QGraphicsSceneMouseEvent *event);
    QPoint pointToPixel(QPoint point);  // 转换为画布上的像素坐标
public:
    QGraphicsScaleItem(QWidget *parent = nullptr);
    ~QGraphicsScaleItem();
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;
    //QPainterPath shape() const Q_DECL_OVERRIDE;
public slots:
    void mouseMove(QPoint point);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
signals:
    void createAuxLine(Qt::Orientation dir);
};

#endif // QGRAPHICSSCALEITEM_H
