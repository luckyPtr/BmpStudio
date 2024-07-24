#ifndef FORMPIXELEDITOR_H
#define FORMPIXELEDITOR_H

#include <QWidget>
#include <QGraphicsScene>
#include "custom/qgraphicsscaleitem.h"
#include "custom/qgraphicscanvasitem.h"
#include "custom/customtab.h"


namespace Ui {
class FormPixelEditor;
}

class FormPixelEditor : public CustomTab
{
    Q_OBJECT

public:
    explicit FormPixelEditor(QWidget *parent = nullptr);
    ~FormPixelEditor();
    QGraphicsCanvasItem *scanvasItem;

    void save() override;
private:
    Ui::FormPixelEditor *ui;

    QImage *image;      // 编辑的图片
    QGraphicsScene *scene;
    QGraphicsScaleItem *scaleItem;

    void initScrollerPos();   // 初始化滚轮位置为左上角
    void initAction();
    void initShortCut();
protected:
    void leaveEvent(QEvent *event) override;

public slots:
    void paintView();
    void on_LoadImage(QImage &image);

signals:
    void updateStatusBarPos(QPoint);
    void saveImage(QImage image);
    void saveImage(QString, int, QImage);
private slots:
    void on_actLockAuxiliaryLine_triggered();
    void on_actHideAuxiliaryLine_triggered();
    void on_actClearAuxiliaryLines_triggered();
    void on_actResize_triggered();
};

#endif // FORMPIXELEDITOR_H
