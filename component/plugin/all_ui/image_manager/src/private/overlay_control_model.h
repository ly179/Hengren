#ifndef OVERLAY_CONTROL_MODEL_H
#define OVERLAY_CONTROL_MODEL_H

enum OverlayStatus {
    OVS_NOT_EXISTS = 0,
    OVS_EXISTS = 1,
    OVS_CHECKED = 2
};

struct OverlayControlModel {
    //显示特征线
    OverlayStatus profileLine = OVS_NOT_EXISTS;
    //显示边缘识别十字光标
    OverlayStatus edgeCrossMark = OVS_NOT_EXISTS;
    //显示设计辅助线
    OverlayStatus designMark = OVS_NOT_EXISTS;
    //显示测量光标标记区域
    OverlayStatus cursorMark = OVS_NOT_EXISTS;
    //显示测量结果
    OverlayStatus viewResult = OVS_NOT_EXISTS;
};

#endif // OVERLAY_CONTROL_MODEL_H
