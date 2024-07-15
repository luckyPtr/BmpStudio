#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>



class Global
{
public:
    Global();

    // 像素编辑器相关
#define AUX_LINE_SCALE      true   // 拖动辅助线时是否显示辅助线刻度
#define ITEM_INFO_FOLLOW    true   // 组合图元素信息显示方块跟随鼠标
    static int pixelSize;          // 像素编辑窗口一个像素的大小
    static int scaleWidth;         // 标尺宽度
    static int scaleOffset;        // 标尺上0点起始位置的偏移
    static int guidesColor;         // 参考线颜色
    static int selectedGuidesColor; // 选择的参考线颜色
    static int pixelColor_0;       // 像素编辑像素值为0的颜色
    static int pixelColor_1;
    static int gridColor;          // 像素编辑器网格颜色
    static int itemBoundColor;      // 未选择的组合图边框颜色
    static int selectedItemBoundColor;  // 选择的组合图边框颜色
    static int selectionBoxColor;       // 选择框颜色
    static bool editMode;           // 编辑模式

    static int exportImgColor_0;    // 导出图片的像素颜色
    static int exportImgColor_1;

    static void initSettings();
private:
    static void createDefaultSettings(const QString &filePath);
    static void readSettings(const QString &filePath);
};

#endif // GLOBAL_H
