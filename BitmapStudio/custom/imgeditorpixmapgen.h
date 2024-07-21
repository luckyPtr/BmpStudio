#ifndef IMGEDITORPIXMAPGEN_H
#define IMGEDITORPIXMAPGEN_H

#include <QObject>
#include <QThread>
#include <QPainter>
#include <QPixmap>

class ImgEditorPixmapGen : public QObject
{
    Q_OBJECT
public:
    explicit ImgEditorPixmapGen(QObject *parent = nullptr);

signals:
    void pixmapReady(const QPixmap &pixmap);

public slots:
    void on_GeneratePixmap(QImage);
};

#endif // IMGEDITORPIXMAPGEN_H
