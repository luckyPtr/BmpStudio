#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QStandardItem>
#include <QVector>
#include <core/projectmng.h>
#include <QLabel>
#include <custom/treeitemdelegate.h>

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

    void on_splitter_splitterMoved(int pos, int index);

    void on_SaveImage(QImage image);

    void on_actEditMode_triggered(bool checked);

    void on_treeViewProject_doubleClicked(const QModelIndex &index);

    void on_actAbout_triggered();


public slots:
    void on_SelectedProjectChanged(QString project);

    void on_UpdateSaveStatus(bool enableSave, bool enableSaveAll);

    void on_UpdateStatusBarPos(QPoint point);

    void on_UpdateStatusBarSize(QSize size);

    void on_UpdatePreview(QImage Image);

    void on_tabWidget_tabCloseRequested(int index);

    void on_SaveComImg(ComImg comImg);

    void on_SaveImage(QString project, int id, QImage image);

    void on_SaveComImg(QString project, int id, ComImg comImg);

    void on_OpenImgTab(QString project, int id);

    void on_ImportFile(QString);

private:
    Ui::MainWindow *ui;

    ProjectMng pm;
    QWidget *formPixelEditor;
    QWidget *formComImgEditor;
    QLabel *labelPosition;
    QLabel *labelSize;
    QLabel *labelPositionIco;
    QLabel *labelSizeIco_new;
    QLabel *labelMode;
    TreeItemDelegate *treeItemDelegate;
    void init();
    void initStatusBar();   // 初始化
    void initAction();

    QModelIndex editedIndex;
    QImage imagePreview;

    void setStackedWidget(int index);
signals:
    void selectItem(QImage &Image);
    void selectItem(ComImg &comImg, RawData *rd);
    void editModeChanged();
};
#endif // MAINWINDOW_H
