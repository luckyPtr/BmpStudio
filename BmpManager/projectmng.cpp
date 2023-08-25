#include "projectmng.h"
#include <string.h>
#include <QInputDialog>


void ProjectMng::addDataNodes(RawData *rd, const quint16 pid, TreeItem *parent, bool (*filter)(int))
{
    QList<BmFile> imgList;

    // 找出图片数据
    QMap<quint16, BmFile> dataMap = rd->getDataMap();
    foreach(const auto &key, dataMap.keys())
    {
        if(filter(dataMap.value(key).type))
        {
            imgList << dataMap.value(key);
        }
    }

    // 按照文件夹>文件，名称字母正序进行排序
    std::sort(imgList.begin(), imgList.end(), [](const BmFile &file1, const BmFile &file2){
        if(file1.type > file2.type) {
            return true;
        }
        else if(file1.type == file2.type) {
            return file1.name.toLower() < file2.name.toLower();
        }
        return false;
    });

    auto addNode = [=](auto&& self, const quint16 pid, TreeItem *parent) -> void{
        foreach(auto bf, imgList)
        {
            if(bf.pid == pid)
            {
                TreeItem *item = new TreeItem();
                item->setID(bf.id);
                item->setType(bf.type);
                item->setRawData(rd);
                parent->addChild(item);
                self(self, bf.id, item);
            }
        }
    };

    addNode(addNode, pid, parent);
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
    // proList的类型从QVector更换为QList，打开三个工程崩溃的问题就没有出现
    // 但是可能并没有真正解决问题，可参考 https://zhidao.baidu.com/question/367115219524964612.html
    projList << pro;        // 惊，还可以这样？？？
}

void ProjectMng::closeProjcet(QModelIndex &index)
{
    projList.removeAt(index.row());
    theModel->removeRow(index);      // 删除model中的项目，否则后面保存TreeView中展开项的时候会崩溃
}

// 根据数据库初始化model
void ProjectMng::initModel()
{
    // TODO: 一次读取数据库，提高速度
    saveExpand();
    theModel->beginReset(); // 需要使用beginReset和endReset通知treeview刷新
    theModel->clear();
    for(int i = 0; i < projList.size(); i++)
    {
        TreeItem *proItem = new TreeItem();
        proItem->setType(RawData::TypeProject);
        proItem->setRawData(&projList[i]);
        proItem->setID(-1);
        theModel->root()->addChild(proItem);

        TreeItem *itemSettings = new TreeItem();
        itemSettings->setType(RawData::TypeClassSettings);
        itemSettings->setRawData(&projList[i]);
        itemSettings->setID(-2);
        proItem->addChild(itemSettings);

        TreeItem *itemImage = new TreeItem();
        itemImage->setType(RawData::TypeClassImg);
        itemImage->setRawData(&projList[i]);
        itemImage->setID(-3);
        proItem->addChild(itemImage);
        addDataNodes(&projList[i], 0, itemImage, RawData::isClassImgType);

        TreeItem *itemComImage = new TreeItem();
        itemComImage->setType(RawData::TypeClassComImg);
        itemComImage->setRawData(&projList[i]);
        itemComImage->setID(-4);
        proItem->addChild(itemComImage);
        addDataNodes(&projList[i], 0, itemComImage, RawData::isClassComImgType);

    }
    theModel->endReset();
    restoreExpand();
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

void ProjectMng::createImage(QModelIndex &index, QString name, quint16 width, quint16 height)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    int id = item->getID();
    int type = item->getType();

    if(type == RawData::TypeClassImg ||\
        type == RawData::TypeImgFolder ||\
        type == RawData::TypeImgGrpFolder ||\
        type == RawData::TypeImgFile)
    {
        rd->createBmp(id, name, width, height);
    }
    else if(type == RawData::TypeClassComImg ||\
            type == RawData::TypeComImgFolder ||\
            type == RawData::TypeComImgFolder)
    {
        rd->createComImg(id, name, QSize(width, height));
    }
}

void ProjectMng::createImage(QModelIndex &index, QString name, QImage &img)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    rd->createBmp(item->getID(), name, img);
}

void ProjectMng::rename(QModelIndex &index, QString name)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    rd->rename(item->getID(), name);
}

void ProjectMng::remove(QModelIndex &index)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    rd->remove(item->getID());
    rd->load();
}

void ProjectMng::imgFolderConvert(QModelIndex &index)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    rd->imgFolderConvert(item->getID());
}

QImage ProjectMng::getImage(QModelIndex index)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    return rd->getImage(item->getID());
}

void ProjectMng::setImage(QModelIndex index, QImage &image)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    rd->setImage(item->getID(), image);
}

ComImg ProjectMng::getComImg(QModelIndex index)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    return rd->getComImg(item->getID());
}

void ProjectMng::setComImg(QModelIndex index, ComImg &comImg)
{
    TreeItem *item = theModel->itemFromIndex(index);
    RawData *rd = item->getRawData();
    rd->setComImg(item->getID(), comImg);
}
