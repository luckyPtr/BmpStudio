#ifndef FORMCOMIMGEDITOR_H
#define FORMCOMIMGEDITOR_H

#include <QWidget>
#include <QGraphicsScene>
#include "custom/qgraphicsscaleitem.h"
#include "custom/qgraphicscomimgcanvansitem.h"
#include "custom/customtab.h"


namespace Ui {
class FormComImgEditor;
}

class FormComImgEditor : public CustomTab
{
    Q_OBJECT

public:
    explicit FormComImgEditor(QWidget *parent = nullptr);
    ~FormComImgEditor();
    QGraphicsComImgCanvansItem * comImgCanvansItem;

    void save() override;
private:
    Ui::FormComImgEditor *ui;

    QGraphicsScene *scene;
    QGraphicsScaleItem *scaleItem;

    void initScrollerPos();   // 初始化滚轮位置为左上角
    void initShortCut();
protected:
    void leaveEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

public slots:
    void on_LoadComImg(ComImg &comImg, RawData *rd);

signals:
    void saveComImg(ComImg comImg);
    void saveComImg(QString, int, ComImg);
    void openImgTab(QString, int);
    void updateStatusBarPos(QPoint);
};

#endif // FORMCOMIMGEDITOR_H
