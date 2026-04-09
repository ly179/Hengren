#ifndef HIGHLIGHT_LOG_TEXTAREA_H
#define HIGHLIGHT_LOG_TEXTAREA_H
#include "cde90_plain_text_edit.h"
#include "log_view_model.h"

struct HighlightLogTextareaHelper;

class CacheLogFlusher;
class HighlightLogTextarea : public CDE90PlainTextEdit{
private:
    HighlightLogTextareaHelper* helper;
    CacheLogFlusher* flusher;

    bool eventFilter(QObject* watch, QEvent*) override;

public:
    explicit HighlightLogTextarea(CacheLogFlusher*, QWidget* parent = nullptr);
    ~HighlightLogTextarea() noexcept;

    void appendLog(const QString&, logview::LogViewLevelEnum = logview::LVLE_ALL);

};

#endif // HIGHLIGHT_LOG_TEXTAREA_H
