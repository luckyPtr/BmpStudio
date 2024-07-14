#include "qcustommenu.h"
#include <QMouseEvent>

QCustomMenu::QCustomMenu(QWidget *parent) : QMenu(parent)
{

}

QCustomMenu::QCustomMenu(const QString &title, QWidget *parent) : QMenu(title, parent)
{

}

void QCustomMenu::mouseReleaseEvent(QMouseEvent *e)
{
     //屏蔽QMenu::mouseReleaseEvent(e)，达到拦截点击的目的，使其点击menu里面任何位置都无效。
    QAction *action = this->actionAt(e->pos());
    if (action)
    {
        action->activate(QAction::Trigger);
    }
    else
    {
        QMenu::mouseReleaseEvent(e);
    }
}
