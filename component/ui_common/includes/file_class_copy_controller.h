#ifndef FILE_CLASS_COPY_CONTROLLER_H
#define FILE_CLASS_COPY_CONTROLLER_H

#include "file_simple_copy_controller.h"
#include "ui_common_global.h"

class UI_COMMON_EXPORT FileClassCopyController : public FileSimpleCopyController{

private:
    QString initData(const SimpleDataCopyParam& param) override;

    QString beginCopy(const SimpleDataCopyParam& param) override;

public:
    explicit FileClassCopyController(QWidget* parent = nullptr);
    ~FileClassCopyController() noexcept;
};

#endif // FILE_CLASS_COPY_CONTROLLER_H
