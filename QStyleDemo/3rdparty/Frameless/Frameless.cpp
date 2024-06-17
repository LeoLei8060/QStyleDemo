#include "Frameless.h"
#include <QApplication>
#include <QDebug>
#include <QScreen>
#include <QWindow>

#include <windows.h>
//
#include <WinUser.h>
#include <dwmapi.h>
#include <windowsx.h>

const QMap<double, int> gDevMap = { { 1, 0 }, { 1.25, 1 }, { 1.5, 2 }, { 1.75, 3 }, { 2, 4 }, { 2.25, 5 }, { 2.5, 6 }, { 3, 7 } };
const QMap<int, int> gDevMaxMap = { { 0, 24 }, { 1, 30 }, { 2, 35 }, { 3, 41 }, { 4, 46 }, { 5, 52 }, { 6, 57 }, { 7, 68 } };
const QMap<int, int> gDevNormalMap = { { 0, 31 }, { 1, 38 }, { 2, 45 }, { 3, 52 }, { 4, 58 }, { 5, 65 }, { 6, 72 }, { 7, 85 } };

static int GetTitleBarHeight_Max()
{
    int index = gDevMap[qApp->devicePixelRatio()];
    return gDevMaxMap[index];
}

static int GetTitleBarHeight_Normal()
{
    int index = gDevMap[qApp->devicePixelRatio()];
    return gDevNormalMap[index];
}

static int GetTitleBarHeight()
{
    qreal titleBarHeight = GetSystemMetrics(SM_CYMENU);

    titleBarHeight = titleBarHeight * qApp->devicePixelRatio();

    return titleBarHeight;
}

Frameless::Frameless(QWidget* parent)
    : QWidget { parent }
{
    FrameNativeEventFilter::instance()->addWindowID(reinterpret_cast<HWND>(this->winId()));
}

bool Frameless::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    if (eventType == "windows_generic_MSG") {
        MSG* msg = reinterpret_cast<MSG*>(message);
        switch (msg->message) {
        case WM_NCCALCSIZE: {
            if (msg->wParam == TRUE) {
                // 修改客户区大小：通过修改客户区顶部位置来隐藏标题栏
                NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)msg->lParam;
                if (::IsZoomed(msg->hwnd)) {
                    params->rgrc[0].top -= GetTitleBarHeight_Max();
                } else {
                    params->rgrc[0].top -= GetTitleBarHeight_Normal();
                }
                return false;
            }
        }
        default:
            break;
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

void Frameless::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange && m_titlebar) {
        if (windowState() & Qt::WindowMaximized) {
            m_titlebar->onMaximized();
        } else {
            m_titlebar->onNormaled();
        }
    }
    QWidget::changeEvent(event);
}

void Frameless::startResizing(const QPoint& pt)
{
    m_bresizing = true;
    m_pressedGeometry = geometry();
    m_pressedPt = pt;
}

void Frameless::moveToResizing(const QPoint& pt)
{
    if (m_bresizing) {
        auto diff = pt - m_pressedPt;
        stretchWidget(diff);
    }
}

void Frameless::stopResizing(const QPoint& pt)
{
    m_bresizing = false;
    m_pressedPt = QPoint();
    m_pressedGeometry = QRect();
}

Frameless::ResizeMode Frameless::getResizeMode()
{
    return m_resizeMode;
}

Frameless::ResizeMode Frameless::getResizeMode(const QPoint& pos)
{
    auto pt = mapFromGlobal(pos);
    const int borderSize = 8; // 拉伸边框的宽度
    ResizeMode mode = NoResize;

    if (isMaximized())
        return mode;

    bool bResizeWidth = minimumWidth() != maximumWidth();
    bool bResizeHeight = minimumHeight() != maximumHeight();

    if (bResizeWidth) {
        if (pt.x() >= 0 && pt.x() <= borderSize)
            mode = LeftResize;
        if (pt.x() >= width() - borderSize && pt.x() <= width())
            mode = RightResize;
    }
    if (bResizeHeight) {
        if (pt.y() >= 0 && pt.y() <= borderSize)
            mode = TopResize;
        if (pt.y() >= height() - borderSize && pt.y() <= height())
            mode = BottomResize;
    }
    if (bResizeWidth && bResizeHeight) {
        if (pt.x() <= borderSize && pt.y() <= borderSize)
            mode = TopLeftResize;
        else if (pt.x() <= borderSize && pt.y() >= height() - borderSize)
            mode = BottomLeftResize;
        else if (pt.x() >= width() - borderSize && pt.y() <= borderSize)
            mode = TopRightResize;
        else if (pt.x() >= width() - borderSize && pt.y() >= height() - borderSize)
            mode = BottomRightResize;
    }

    m_resizeMode = mode;
    return mode;
}

void Frameless::setResizeCursor(ResizeMode mode)
{
    Qt::CursorShape cursorShape;
    switch (mode) {
    case TopResize:
    case BottomResize:
        cursorShape = Qt::SizeVerCursor;
        break;
    case LeftResize:
    case RightResize:
        cursorShape = Qt::SizeHorCursor;
        break;
    case TopLeftResize:
    case BottomRightResize:
        cursorShape = Qt::SizeFDiagCursor;
        break;
    case TopRightResize:
    case BottomLeftResize:
        cursorShape = Qt::SizeBDiagCursor;
        break;
    default:
        cursorShape = Qt::ArrowCursor;
        break;
    }
    setCursor(cursorShape);
}

void Frameless::stretchWidget(const QPoint& diff)
{
    if (m_pressedGeometry.isEmpty())
        return;
    const int minWidth = minimumWidth();
    const int minHeight = minimumHeight();
    auto rect = m_pressedGeometry;
    switch (m_resizeMode) {
    case TopResize: {
        int newTop = rect.top() + diff.y();
        int newHeight = rect.height() - diff.y();
        if (newHeight < minHeight)
            newTop -= (minHeight - newHeight);
        setGeometry(rect.left(), newTop, rect.width(), newHeight);
    } break;
    case RightResize: {
        int newWidth = rect.width() + diff.x();
        if (newWidth < minWidth)
            newWidth = minWidth;
        setGeometry(rect.left(), rect.top(), newWidth, rect.height());
    } break;
    case BottomResize: {
        int newHeight = rect.height() + diff.y();
        if (newHeight < minHeight)
            newHeight = minHeight;
        setGeometry(rect.left(), rect.top(), rect.width(), newHeight);
    } break;
    case LeftResize: {
        int newLeft = rect.left() + diff.x();
        int newWidth = rect.width() - diff.x();
        if (newWidth < minWidth)
            newLeft -= (minWidth - newWidth);
        setGeometry(newLeft, rect.top(), newWidth, rect.height());
    } break;
    case TopLeftResize: {
        int newLeft = rect.left() + diff.x();
        int newTop = rect.top() + diff.y();
        int newWidth = rect.width() - diff.x();
        int newHeight = rect.height() - diff.y();
        if (newWidth < minWidth)
            newLeft -= (minWidth - newWidth);
        if (newHeight < minHeight)
            newTop -= (minHeight - newHeight);
        setGeometry(newLeft, newTop, newWidth, newHeight);
    } break;
    case BottomRightResize: {
        int newWidth = rect.width() - diff.x();
        int newHeight = rect.height() - diff.y();
        if (newWidth < minWidth)
            newWidth = minWidth;
        if (newHeight < minHeight)
            newHeight = minHeight;
        setGeometry(rect.left(), rect.top(), newWidth, newHeight);
    } break;
    case TopRightResize: {
        int newTop = rect.top() + diff.y();
        int newWidth = rect.width() - diff.x();
        int newHeight = rect.height() - diff.y();
        if (newWidth < minWidth)
            newWidth = minWidth;
        if (newHeight < minHeight)
            newTop -= (minHeight - newHeight);
        setGeometry(rect.left(), newTop, newWidth, newHeight);
    } break;
    case BottomLeftResize: {
        int newLeft = rect.left() + diff.x();
        int newWidth = rect.width() - diff.x();
        int newHeight = rect.height() + diff.y();
        if (newWidth < minWidth)
            newLeft -= (minWidth - newWidth);
        if (newHeight < minHeight)
            newHeight = minHeight;
        setGeometry(newLeft, rect.top(), newWidth, newHeight);
    } break;
    default:
        break;
    }
}
