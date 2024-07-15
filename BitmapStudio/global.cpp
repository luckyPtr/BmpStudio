#include "global.h"
#include <QSettings>
#include <QFile>
#include <QCoreApplication>
#include <QDebug>

int Global::pixelSize = 8;
int Global::scaleWidth = 16;
int Global::scaleOffset = 4;
int Global::pixelColor_1 = 0x9ce0ef;
int Global::pixelColor_0 = 0x495028;
int Global::gridColor = 0x303030;
int Global::itemBoundColor = 0x808080;
int Global::selectedItemBoundColor = 0x00FFFF;
int Global::selectionBoxColor = 0xFFFF00;
int Global::guidesColor = 0xFFFF00;
int Global::selectedGuidesColor = 0xFF8C00;
bool Global::editMode = false;

int Global::exportImgColor_0 = 0xFFFFFF;
int Global::exportImgColor_1 = 0x000000;




void Global::initSettings()
{
    QString iniFilePath = QCoreApplication::applicationDirPath() + "/config.ini";
    // 判断文件是否存在
    if (!QFile::exists(iniFilePath)) {
        // 文件不存在，创建默认配置
        createDefaultSettings(iniFilePath);
    } else {
        // 文件存在，读取配置
        readSettings(iniFilePath);
    }
}

void Global::createDefaultSettings(const QString &filePath)
{
    // 创建QSettings对象，指定文件路径和格式
    QSettings settings(filePath, QSettings::IniFormat);

    settings.beginGroup("General");
    settings.setValue("export_image_color_0", QString::number(exportImgColor_0, 16).toUpper());
    settings.setValue("export_image_color_1", QString::number(exportImgColor_1, 16).toUpper());
    settings.endGroup();
}

void Global::readSettings(const QString &filePath)
{
    // 创建QSettings对象，指定文件路径和格式
    QSettings settings(filePath, QSettings::IniFormat);

    settings.beginGroup("General");
    bool ok;
    exportImgColor_0 = settings.value("export_image_color_0", QString::number(exportImgColor_0, 16)).toString().toInt(&ok, 16);
    exportImgColor_1 = settings.value("export_image_color_1", QString::number(exportImgColor_1, 16)).toString().toInt(&ok, 16);
    settings.endGroup();
}
