#include "projectmng.h"
#include <string.h>
#include <QInputDialog>

void ProjectMng::addImgNode(RawData *rd, const quint16 pid, TreeItem *parent)
{
    QMap<quint16, BmImg> imgMap = rd->getImgMap();
    foreach(auto bi, imgMap)
    {
        if(bi.pid == pid)
        {
            TreeItem *item = new TreeItem();
            item->setID(bi.id);
            item->setProject(rd->getProject());
            item->setType(bi.isFolder ? TreeItem::FOLDER : TreeItem::FILE);
            item->setRawData(rd);
            parent->addChild(item);
            addImgNode(rd, bi.id, item);
        }
    }
}

void ProjectMng::getExpandNode(QModelIndex root)
{
    TreeItem *item = theModel->itemFromIndex(root);
    quint16 id = item->getID();
    if(treeView->isExpanded(root))
    {
        item->getRawData()->addExpandNode(id);
    }
    for(int i = 0; i < theModel->rowCount(root); i++)
    {
        QModelIndex index = theModel->index(i, 0, root);
        getExpandNode(index);
    }
}

void ProjectMng::setExpandNode(QModelIndex root)
{
    TreeItem *item = theModel->itemFromIndex(root);
    quint16 id = item->getID();
    if(item->getRawData()->isExpandNode(id))
    {
        treeView->expand(root);
    }
    for(int i = 0; i < theModel->rowCount(root); i++)
    {
        QModelIndex index = theModel->index(i, 0, root);
        setExpandNode(index);
    }
}

void ProjectMng::saveExpand()
{
    for(int i = 0; i < theModel->rowCount(treeView->rootIndex()); i++)
    {
        QModelIndex root = theModel->index(i, 0, treeView->rootIndex());
        TreeItem *item = theModel->itemFromIndex(root);
        //projList[i].expand.clear();
        item->getRawData()->clearExpandNode();
        getExpandNode(root);
    }
}

void ProjectMng::restoreExpand()
{
    qDebug() << theModel->rowCount(treeView->rootIndex());

    for(int i = 0; i < theModel->rowCount(treeView->rootIndex()); i++)
    {
        QModelIndex root = theModel->index(i, 0, treeView->rootIndex());
        setExpandNode(root);
    }
}



ProjectMng::ProjectMng(QWidget *parent)
    : QWidget{parent}
{
    theModel = new TreeModel(this);

}

void ProjectMng::openProject(QString pro)
{
    projList << pro;        // 惊，还可以这样？？？
}

// 根据数据库初始化model
void ProjectMng::initModel()
{
    // TODO: 一次读取数据库，提高速度
    QElapsedTimer timer;
    timer.start();
    saveExpand();
    theModel->beginReset(); // 需要使用beginReset和endReset通知treeview刷新
    theModel->clear();
    for(int i = 0; i < projList.size(); i++)
    {
        TreeItem *proItem = new TreeItem();
        proItem->setType(TreeItem::PROJECT);
        proItem->setRawData(&projList[i]);
        proItem->setID(-1);
        theModel->root()->addChild(proItem);

        TreeItem *itemSettings = new TreeItem();
        itemSettings->setType(TreeItem::CLASS_SETTINGS);
        itemSettings->setRawData(&projList[i]);
        itemSettings->setID(-2);
        proItem->addChild(itemSettings);

        TreeItem *itemImage = new TreeItem();
        itemImage->setType(TreeItem::CLASS_IMAGE);
        itemImage->setRawData(&projList[i]);
        itemImage->setID(0);
        proItem->addChild(itemImage);
        addImgNode(&projList[i], 0, itemImage);

        TreeItem *itemComImage = new TreeItem();
        itemComImage->setType(TreeItem::CLASS_COMIMAGE);
        itemComImage->setRawData(&projList[i]);
        itemComImage->setID(10000);
        proItem->addChild(itemComImage);
    }
    theModel->endReset();
    restoreExpand();

    qDebug() << "耗时" << timer.elapsed();
}

void ProjectMng::blindTreeView(QTreeView *treeView)
{
    this->treeView = treeView;
    treeView->setModel(theModel);
}


void ProjectMng::createFolder(QModelIndex index)
{
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("新建文件夹"), tr("文件夹名"), QLineEdit::Normal, "", &ok);
    if(ok && !text.isEmpty())
    {
        TreeItem *item = theModel->itemFromIndex(index);
        RawData *rd = item->getRawData();
        rd->createFolder(item->getID(), text);
    }
}




