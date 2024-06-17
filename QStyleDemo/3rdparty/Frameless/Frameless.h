#ifndef FRAMELESS_H
#define FRAMELESS_H

#include "FrameEventFilter.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QWidget>

class FramelessTitle : public QWidget {
    Q_OBJECT
public:
    explicit FramelessTitle(QWidget* parent = nullptr)
        : QWidget(parent)
    {
    }

    /**
     * @brief: 鼠标位置是否在标题栏内
     * @return: return true表示鼠标在标题栏内，可以移动窗口
     */
    virtual bool inCaption(const QPoint& pt) { return false; }

public slots:
    virtual void onMaximized() { }
    virtual void onNormaled() { }
};

class Frameless : public QWidget {
    Q_OBJECT
public:
    enum ResizeMode {
        NoResize,
        TopResize,
        BottomResize,
        LeftResize,
        RightResize,
        TopLeftResize,
        TopRightResize,
        BottomLeftResize,
        BottomRightResize
    };
    explicit Frameless(QWidget* parent = nullptr);

    void setTitlebar(FramelessTitle* titlebar) { m_titlebar = titlebar; }

    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

    void changeEvent(QEvent* event) override;

    void setResizeFlag(bool bResize) { m_bresizeFlag = bResize; }

    void startResizing(const QPoint& pt);
    void moveToResizing(const QPoint& pt);
    void stopResizing(const QPoint& pt);

    ResizeMode getResizeMode();
    ResizeMode getResizeMode(const QPoint& pos);
    void setResizeCursor(ResizeMode mode);

    void stretchWidget(const QPoint& diff);

private:
    ResizeMode m_resizeMode = NoResize;

    bool m_bresizing = false;
    bool m_bresizeFlag = false;
    QPoint m_pressedPt;
    QRect m_pressedGeometry;

    FramelessTitle* m_titlebar { nullptr };
};

#endif // FRAMELESS_H
