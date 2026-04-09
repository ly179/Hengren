#include "chip_map_helper.h"
#include "idw_canvas.h"
#include "idw_wafer_data_model.h"

ChipMapHelper::ChipMapHelper(
    IdwCanvas* canvas)
    : canvas(canvas) {
    localColumnSortFunc = std::bind(&ChipMapHelper::localSortByColumn,
                                    this,
                                    std::placeholders::_1,
                                    std::placeholders::_2);
    localRowSortFunc = std::bind(&ChipMapHelper::localSortByRow,
                                 this,
                                 std::placeholders::_1,
                                 std::placeholders::_2);
}

bool ChipMapHelper::localSortByColumn(
    int number1, int number2) {
    QPoint p1 = canvas->localFindChipRulerByNumber(number1);
    QPoint p2 = canvas->localFindChipRulerByNumber(number2);
    if (p1.x() < p2.x()) {
        return true;
    }
    if (p1.x() > p2.x()) {
        return false;
    }
    return p1.y() < p2.y();
}

bool ChipMapHelper::localSortByRow(
    int number1, int number2) {
    QPoint p1 = canvas->localFindChipRulerByNumber(number1);
    QPoint p2 = canvas->localFindChipRulerByNumber(number2);
    if (p1.y() < p2.y()) {
        return true;
    }
    if (p1.y() > p2.y()) {
        return false;
    }
    return p1.x() < p2.x();
}

ChipMapHandlerResult ChipMapHelper::localCreateRowColumnIndex() {
    QList<IDW_Data_Item>* shotList = canvas->localShot()->getDataList();
    QList<IDW_Data_Item>* chipList = canvas->localChip()->getDataList();
    const QSet<qint32>& invalidNumbers = canvas->localGetInvalidChipNumbers();
    if (!shotList || !chipList) {
        return {};
    }
    bool succ;
    int chipNumber;
    QPoint p;
    for (IDW_Data_Item& shotItem : *shotList) {
        for (IDW_Data_Item& chipItem : *chipList) {
            chipNumber = getChipNumberByShotChipNumber(shotItem.getNumber(),
                                                       chipItem.getNumber(),
                                                       canvas->localChip().data(),
                                                       &succ);
            if (!succ) {
                perColumnMap.clear();
                perRowMap.clear();
                return {"Error", "Index chip (x,y) error.", true};
            }
            if (invalidNumbers.contains(chipNumber)) {
                // p = CANVAS->localFindChipRulerByNumber(chipNumber, &succ);
                continue;
            }
            p = canvas->localFindChipRulerByNumber(chipNumber, &succ);
            if (!succ) {
                perColumnMap.clear();
                perRowMap.clear();
                return {"Error", "Index chip (x,y) error, chip info lost.", true};
            }
            perColumnMap[p.x()][p.y()].append(chipNumber);
            perRowMap[p.y()][p.x()].append(chipNumber);
        }
    }
    return {};
}

bool customStep(
    int step,
    QHash<qint32, QMap<qint32, QList<qint32>>>::Iterator& it,
    QMap<qint32, QList<qint32>>::Iterator& cit) {
    while (step != 0) {
        if (step > 0) {
            --step;
            ++cit;
            if (cit == it->end()) {
                return false;
            }
            continue;
        }
        ++step;
        --cit;
        if (cit == it->end()) {
            return false;
        }
    }
    return true;
}

ChipMapHandlerResult ChipMapHelper::localShiftWork(
    int id, Qt::Orientation direction, int step, bool keepOld) {
    IdwLinkedList* list = canvas->localGetCheckedPoints(id);
    if (list->isEmpty()) {
        return {"", "", false, false};
    }
    if (perColumnMap.isEmpty()) {
        localCreateRowColumnIndex();
    }
    QList<QPoint> numberRules;
    numberRules.reserve(list->size());
    const IdwLinkedNode* node = list->first();
    QPoint p;
    bool succ;
    while (node) {
        p = canvas->localFindChipRulerByNumber(node->data, &succ);
        if (!succ) {
            return {"Error", "A Chip info is lost.", true};
        }
        numberRules.append(p);
        node = node->next;
    }

    QList<qint32> waitReselectedNumbers;
    //waitReselectedNumbers的容量可能会不够还需要继续扩容。
    waitReselectedNumbers.reserve(numberRules.size());

    int realStep = std::abs(step);
    //左右移
    if (direction == Qt::Horizontal) {
        //check could move
        for (QPoint& pos : numberRules) {
            auto it = perRowMap.find(pos.y());
            if (it == perRowMap.end()) {
                return {"", "", false, false};
            }
            auto cit = it->lowerBound(pos.x());
            if (!customStep(step, it, cit)) {
                return {"", "", false, false};
            }
            if (std::abs(cit.key() - pos.x()) != realStep) {
                return {"", "", false, false};
            }
            waitReselectedNumbers.append((*cit));
        }
    } else {
        //check could move
        for (QPoint& pos : numberRules) {
            auto it = perColumnMap.find(pos.x());
            if (it == perColumnMap.end()) {
                return {"", "", false, false};
            }
            auto cit = (*it).lowerBound(pos.y());
            if (!customStep(step, it, cit)) {
                return {"", "", false, false};
            }
            if (std::abs(cit.key() - pos.y()) != realStep) {
                return {"", "", false, false};
            }
            waitReselectedNumbers.append((*cit));
        }
    }
    if (!keepOld)
        list->clear();
    for (qint32& number : waitReselectedNumbers) {
        canvas->localCheckItem(id, number, 1);
    }
    return {"", "", false, waitReselectedNumbers.size() > 0};
}
