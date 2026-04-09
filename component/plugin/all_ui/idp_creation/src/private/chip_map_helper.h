#ifndef CHIP_MAP_HELPER_H
#define CHIP_MAP_HELPER_H

#include <QHash>
#include <QList>

struct ChipMapHandlerResult {
    QString title;
    QString msg;
    bool error = false;
    bool moved = false;
};

class IdwCanvas;
class ChipMapHelper {
private:
    std::function<bool(int, int)> localColumnSortFunc;
    std::function<bool(int, int)> localRowSortFunc;

    //该功能是IDW_Canvas中根据chip编号查询它的标尺的反向查询操作
    //IDW_Canvas画布提供了大多数有效的API，但是有些操作非画布必要功能，而是外部应用特有功能
    //因此这部分功能不放在画布中实现，否则在其他非特定场景会造成性能损耗。
    //不规则的排列可能会导致chip重叠，重叠后得到的标尺坐标相同，因此使用Qlist作为value，但是
    //大多数情况下Qlist的size都为1.
    //每一行每一行的number排序，如0 - 2-1  表示第一行第三列的chip编号为1.
    QHash<qint32, QMap<qint32, QList<qint32>>> perRowMap;
    //与perRowMap功能相同，为每一列每一列。
    QHash<qint32, QMap<qint32, QList<qint32>>> perColumnMap;
    IdwCanvas* canvas;

    //这个是为std::sort提供的比较器，用于排序number
    bool localSortByColumn(int number1, int number2);
    bool localSortByRow(int number1, int number2);
    ChipMapHandlerResult localCreateRowColumnIndex();
    //shift select, d = u(up), r(right), d(down), l(left)

public:
    explicit ChipMapHelper(IdwCanvas* canvas);

    ChipMapHandlerResult localShiftWork(int id, Qt::Orientation, int step, bool keepOld = false);

    const std::function<bool(int, int)>& getLocalColumnSortFunc() const {
        return localColumnSortFunc;
    }
    const std::function<bool(int, int)>& getLocalRowSortFunc() const { return localRowSortFunc; }
};

#endif // CHIP_MAP_HELPER_H
