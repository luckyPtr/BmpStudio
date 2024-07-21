#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QObject>
#include <QApplication>
#include <QLocalServer>
#include <QMouseEvent>
#include <QDebug>

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int& argc, char** argv);
    bool isRunning();

private:
    const QString serverName = "BitmapStudioServer";
    bool m_IsRunning = false;
    QLocalServer *m_server;
    QString filePath = nullptr;

    void initConnect();
    void newLocalServer();
    void newLocalConnect();
    void activeMainWindow();
signals:
    void importFile(QString);
};

#endif // SINGLEAPPLICATION_H
