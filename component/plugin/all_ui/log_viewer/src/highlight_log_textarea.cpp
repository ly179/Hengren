#include "highlight_log_textarea.h"

#include <cde90_menu.h>
#include <qclipboard.h>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QScrollBar>
#include <sem_global.h>

#include "cache_log_flusher.h"

using namespace logview;
using namespace cache_log_viewer;

class LogLineBlockData : public QTextBlockUserData {
public:

    LogLineData configData;

    QList<std::pair<int, int>> sames;

    ~LogLineBlockData() override {}
};

class LogHighLight : public QSyntaxHighlighter {
private:
    QTextCharFormat charFormat;
    CacheLogFlusher* flusher;
protected:

    void highlightBlock(
        const QString& text) override {
        QTextBlock block = currentBlock();
        LogLineBlockData* blockData = dynamic_cast<LogLineBlockData *>(block.userData());
        //block不存在，log level按照all来运算
        if (!blockData) {
            block.setUserData(blockData = new LogLineBlockData);
            LogContent c;
            c.text = block.text();
            flusher->formatHightLight(c);
            blockData->configData = c.formatData;
        }
        for (ColorConfig& config : blockData->configData.configs) {
            setFormat(config.start,
                      config.len,
                      config.color);
        }
        for (const std::pair<int, int>& pair : blockData->sames) {
            setFormat(pair.first, pair.second, charFormat);
        }
    }

public:
    explicit LogHighLight(CacheLogFlusher* flusher, QTextDocument* parent = nullptr)
        : QSyntaxHighlighter(parent), flusher(flusher) {
        charFormat.setTextOutline(QPen(QColor(255, 0, 0), 1));
        charFormat.setFontWeight(true);
    }
};

struct HighlightLogTextareaHelper{
    QTextCursor cursor;
    LogHighLight* highLight;
    CacheLogFlusher* flusher;
    HighlightLogTextarea* d;

    int perClearNumber = 2000;
    int allowLineNumber = 10000;

    bool keepDown = false;

    void checkAndClearOutOfMemoryLines() {
        int blockCount = highLight->document()->blockCount();
        if (blockCount <= allowLineNumber) {
            return;
        }
        QTextCursor cursor = d->textCursor();
        QTextDocument* doc = d->document();
        cursor.setPosition(0);
        QTextBlock block = doc->findBlockByLineNumber(perClearNumber - 1);
        if (!block.isValid()) {
            return;
        }
        cursor.setPosition(block.position() + block.length(), QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }

    void insertTextBlock(QTextCursor& cursor, const QString text, LogViewLevelEnum level) {
        if (cursor.block().length() > 1)
            cursor.insertBlock();
        cursor.insertText(text);
        QTextBlock block = cursor.block();
        LogLineBlockData* blockData = dynamic_cast<LogLineBlockData *>(block.userData());
        //block不存在，log level按照all来运算
        if (!blockData) {
            block.setUserData(blockData = new LogLineBlockData);
            LogContent c;
            c.formatData.level = level;
            c.text = text;
            flusher->formatHightLight(c);
            blockData->configData = c.formatData;
        }
    }

    void beginFetch(bool append, bool begin) {
        if (begin) {
            if (!append) {
                d->clear();
                keepDown = true;
            }
            keepDown = std::abs(d->verticalScrollBar()->value() - d->verticalScrollBar()->maximum()) < 3;
            cursor = d->textCursor();
            cursor.beginEditBlock();
            cursor.movePosition(QTextCursor::End);
        } else {
            checkAndClearOutOfMemoryLines();
            cursor.endEditBlock();
            if (keepDown)
                d->verticalScrollBar()->setValue(d->verticalScrollBar()->maximum());
        }
    }

    void appendLogContent(const LogViewSearchResult& result) {
        if (cursor.block().length() > 1)
            cursor.insertBlock();
        cursor.insertText(result.logData.text);
        QTextBlock block = cursor.block();
        LogLineBlockData* blockData = dynamic_cast<LogLineBlockData *>(block.userData());
        if (!blockData) {
            block.setUserData(blockData = new LogLineBlockData);
        }
        blockData->configData = result.logData.formatData;
        blockData->sames = result.sames;
    }
};

HighlightLogTextarea::HighlightLogTextarea(CacheLogFlusher* flusher, QWidget* parent) : CDE90PlainTextEdit(parent), flusher(flusher){
    installEventFilter(this);
    setStyleSheet("font-weight: normal;");
    helper = new HighlightLogTextareaHelper;
    helper->d = this;
    helper->highLight = new LogHighLight(flusher, document());
    helper->flusher = flusher;

    QPointer _this(this);
    flusher->setFetchCallback([_this](bool append, bool begin) {
        if (!MainThreadInvoker::instance()->isMainThread()) {
            MainThreadInvoker::instance()->runLater([_this, append, begin]() {
                if (!_this) return;
                _this->helper->beginFetch(append, begin);
            });
            return;
        }
        _this->helper->beginFetch(append, begin);
    });
    flusher->setContentCallback([_this](const LogViewSearchResult& result) {
        if (!MainThreadInvoker::instance()->isMainThread()) {
            MainThreadInvoker::instance()->runLater([_this,result]() {
                if (!_this) return;
                _this->helper->appendLogContent(result);
            });
            return;
        }
        _this->helper->appendLogContent(result);
    });

    setContextMenuPolicy(Qt::CustomContextMenu);
    CDE90Menu* menu = new CDE90Menu(this);
    QAction* action = menu->addAction("Select All");
    connect(action, &QAction::triggered, this, &HighlightLogTextarea::selectAll);
    action = new QAction(this);
    action->setSeparator(true);
    menu->addAction(action);
    action = menu->addAction("Copy");
    connect(action, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        QString text = cursor.selectedText();
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(text);
    });
    connect(this, &HighlightLogTextarea::customContextMenuRequested, this, [menu, action, this](const QPoint& pos) {
        QTextCursor cursor = textCursor();
        action->setEnabled(cursor.hasSelection());
        menu->popup(QWidget::mapToGlobal(pos));
    });
}
HighlightLogTextarea::~HighlightLogTextarea() noexcept {
    flusher->setFetchCallback(nullptr);
    flusher->setContentCallback(nullptr);
    delete helper->highLight;
    delete helper;
}

bool HighlightLogTextarea::eventFilter(QObject* watched, QEvent* event) {
    if (watched != this) return CDE90PlainTextEdit::eventFilter(watched, event);
    if (event->type() == QEvent::InputMethod || event->type() == QEvent::InputMethodQuery)
        return true;
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        // 允许：标准快捷键 (复制、全选)
        if (keyEvent->matches(QKeySequence::Copy) ||
            keyEvent->matches(QKeySequence::SelectAll)) {
            return CDE90PlainTextEdit::eventFilter(watched, event);
        }
        switch (keyEvent->key()) {
            case Qt::Key_Left:
            case Qt::Key_Right:
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Home:
            case Qt::Key_End:
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
            case Qt::Key_Control: // 允许 Ctrl 键按下（为了组合键）
            case Qt::Key_Shift:   // 允许 Shift 键按下（为了选择文本）
                return CDE90PlainTextEdit::eventFilter(watched, event);
            default:
                break;
        }
        if (keyEvent->matches(QKeySequence::Paste) ||
            keyEvent->matches(QKeySequence::Cut) ||
            keyEvent->matches(QKeySequence::Undo) ||
            keyEvent->matches(QKeySequence::Redo) ||
            keyEvent->matches(QKeySequence::Delete) ||
            keyEvent->key() == Qt::Key_Backspace ||
            keyEvent->key() == Qt::Key_Return ||
            keyEvent->key() == Qt::Key_Enter) {
            return true;
            }

        // 拦截所有带文本内容的输入（字符、数字、符号）
        if (!keyEvent->text().isEmpty()) {
            return true;
        }

        // 剩下的其他未知按键默认拦截，或者你可以根据需要放行
        return true;
    }

    return CDE90PlainTextEdit::eventFilter(watched, event);
}

void HighlightLogTextarea::appendLog(const QString& text, logview::LogViewLevelEnum level) {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    helper->insertTextBlock(cursor, text, level);;
    helper->checkAndClearOutOfMemoryLines();
    cursor.endEditBlock();
    // appendPlainText(text);
}