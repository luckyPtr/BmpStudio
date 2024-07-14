#ifndef QCUSTOMMENU_H
#define QCUSTOMMENU_H

#include <QMenu>
#include <QWidget>

// 自定义Menu，菜单触发后不会回弹
// https://blog.csdn.net/weixin_43246170/article/details/126279107
class QCustomMenu : public QMenu
{
    Q_OBJECT
public:
    QCustomMenu(QWidget *parent = nullptr);
    QCustomMenu(const QString &title, QWidget *parent = nullptr);
protected:
    void mouseReleaseEvent(QMouseEvent *e);
};

#endif // QCUSTOMMENU_H
