#ifndef QGRAPHICSITEMBASE_H
#define QGRAPHICSITEMBASE_H

#include <QGraphicsItem>
#include <QObject>
#include <QVector>


class QGraphicsItemBase : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    struct AuxiliaryLine
    {
        Qt::Orientation dir;
        int scale;
        AuxiliaryLine(Qt::Orientation dir, int scale)
        {
            this->dir = dir;
            this->scale = scale;
        }
    };

    int selectedItemIndex = -1;
    static QVector<AuxiliaryLine> auxiliaryLines;

    explicit QGraphicsItemBase(QGraphicsItem *parent = nullptr);

};

#endif // QGRAPHICSITEMBASE_H
