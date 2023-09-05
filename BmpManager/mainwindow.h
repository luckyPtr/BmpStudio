#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QStandardItem>
#include <QVector>
#include <projectmng.h>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_actOpenProject_triggered();

    void on_treeViewProject_clicked(const QModelIndex &index);

    void on_actNewFolder_triggered();

    void on_actRename_triggered();

    void on_actNewProject_triggered();

    void on_actNewImg_triggered();

    void on_splitter_splitterMoved(int pos, int index);

    void on_actImportImg_triggered();

    void on_actDelete_triggered();

    void on_actTest_triggered();

    void on_SaveImage(QImage image);

    void on_SaveComImg(ComImg comImg);

    void on_actGrpImgTransform_triggered();

    void on_actRun_triggered();

    void on_StatusBarInfo(QPoint point, QSize size);

    void on_actCopyName_triggered();

private:
    Ui::MainWindow *ui;

    ProjectMng pm;
    QWidget *formPixelEditor;
    QWidget *formComImgEditor;
    QLabel *labelPosition;
    QLabel *labelSize;
    QLabel *labelPositionIco;
    QLabel *labelSizeIco;
    void init();

    QModelIndex editedIndex;

    enum
    {
        STACKED_WIDGET_DEFAULT,
        STACKED_WIDGET_COMIMG,
        STACKED_WIDGET_IMG,
    };

    void setStackedWidget(int index);

signals:
    void selectItem(QImage &Image);

    void selectItem(ComImg &comImg, RawData *rd);
};
#endif // MAINWINDOW_H
