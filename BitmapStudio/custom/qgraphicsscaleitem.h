#ifndef QGRAPHICSSCALEITEM_H
#define QGRAPHICSSCALEITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QDebug>

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
    static bool isLock;
    static bool isHide;
    int selectedAuxiliaryLine = -1;     // 选中的辅助线
    QPoint startPoint;      // 画布左上角起始坐标
    QPointF mousePos;
    QPoint mousePixel;      // 鼠标所在的像素坐标
    Qt::MouseButton mouseButton;
    int createGuidesStep = 0;   // 0-默认 1-在水平标尺区域按下 2-在垂直标尺区域按下 3-拖到画布区域
    void drawScale(QPainter *painter);
    void drawAuxiliaryLines(QPainter *painter);
    bool isScaleArea(QPoint point);     // 是否在刻度尺区域
    bool isHorizontalScale(QPoint point);   // 是否在水平刻度线
    bool isVerticalScale(QPoint point);     // 是否在垂直的刻度线区域内
    int getNearGuide(QPoint point);         // 获取附近的参考线
    void sendEventToOtherItems(QGraphicsSceneMouseEvent *event);
    QPoint pointToPixel(QPoint point);  // 转换为画布上的像素坐标
public:
    QGraphicsScaleItem(QWidget *parent = nullptr);
    ~QGraphicsScaleItem();
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;
    //QPainterPath shape() const Q_DECL_OVERRIDE;
protected:

public slots:
    void mouseMove(QPoint point);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void on_DeleteAllGuides();
    void on_LockGuides();
    void on_HideGuides();
    void on_DeleteGuide();
signals:
    void createAuxLine(Qt::Orientation dir);
};

#endif // QGRAPHICSSCALEITEM_H
