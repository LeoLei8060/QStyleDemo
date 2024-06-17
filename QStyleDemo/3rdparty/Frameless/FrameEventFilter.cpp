#include "FrameEventFilter.h"
#include "frameless.h"
#include <QGuiApplication>
#include <QRect>
#include <QScreen>
#include <QWindow>

//
#include <WinUser.h>
#include <dwmapi.h>
#include <windowsx.h>

#define TOP_RESIZE_MARGIN 4 // 标题栏顶部用于resize模式的边距

FrameEventFilter::FrameEventFilter(QObject *parent)
    : QObject{parent}
{}

bool FrameEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        auto msEvent = static_cast<QMouseEvent *>(event);
        // qDebug() << "MouseMove:";
        if (msEvent->buttons() & Qt::LeftButton) {
            for (auto frame : m_resizeWidgets) {
                if (frame->isActiveWindow() && frame->getResizeMode() != Frameless::ResizeMode::NoResize) {
                    frame->moveToResizing(msEvent->globalPos());
                    return true;
                }
            }
        } else {
            for (auto frame : m_resizeWidgets) {
                frame->setResizeCursor(frame->getResizeMode(static_cast<QMouseEvent *>(event)->globalPos()));
            }
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
        auto msEvent = static_cast<QMouseEvent *>(event);
        if (msEvent->button() == Qt::LeftButton) {
            for (auto frame : m_resizeWidgets) {
                if (frame->getResizeMode(msEvent->globalPos()) != Frameless::ResizeMode::NoResize) {
                    frame->startResizing(msEvent->globalPos());
                    break;
                }
            }
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        auto msEvent = static_cast<QMouseEvent *>(event);
        if (msEvent->button() == Qt::LeftButton) {
            for (auto frame : m_resizeWidgets) {
                if (frame->getResizeMode() != Frameless::ResizeMode::NoResize) {
                    frame->stopResizing(msEvent->globalPos());
                    break;
                }
            }
        }
    }
    return QObject::eventFilter(watched, event);
}

FrameNativeEventFilter *FrameNativeEventFilter::instance()
{
    static FrameNativeEventFilter *instance = new FrameNativeEventFilter;
    return instance;
}

bool FrameNativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG") {
        MSG *msg = reinterpret_cast<MSG *>(message);
        switch (msg->message) {
        case WM_NCHITTEST: {
            *result = 0;
            // if (!isExist_winId(msg->hwnd))
            //     return false;

            long  x = GET_X_LPARAM(msg->lParam);
            long  y = GET_Y_LPARAM(msg->lParam);
            POINT nativeLocalPos{x, y};

            ::ScreenToClient(msg->hwnd, &nativeLocalPos);
            QPoint pt(nativeLocalPos.x, nativeLocalPos.y);
            if (m_windows.indexOf(msg->hwnd) != -1) {
                const int borderWidth = 5;

                RECT rc;
                GetClientRect(msg->hwnd, &rc);
                QRect m_widgetRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);

                bool bResizeWidth = true;
                bool bResizeHeight = true;

                if (bResizeWidth) {
                    // left border
                    if (pt.x() >= m_widgetRect.left() && pt.x() < m_widgetRect.left() + borderWidth)
                        *result = HTLEFT;
                    // right border
                    if (pt.x() < m_widgetRect.right() && pt.x() >= m_widgetRect.right() - borderWidth)
                        *result = HTRIGHT;
                }
                if (bResizeHeight) {
                    // top border
                    if (pt.y() >= m_widgetRect.top() && pt.y() < m_widgetRect.top() + borderWidth)
                        *result = HTTOP;
                    // bottom border
                    if (pt.y() < m_widgetRect.bottom() && pt.y() >= m_widgetRect.bottom() - borderWidth)
                        *result = HTBOTTOM;
                }
                if (bResizeWidth && bResizeHeight) {
                    // left top corner
                    if (pt.x() >= m_widgetRect.left() && pt.x() < m_widgetRect.left() + borderWidth
                        && pt.y() >= m_widgetRect.top() && pt.y() < m_widgetRect.top() + borderWidth)
                        *result = HTTOPLEFT;
                    // right top corner
                    if (pt.x() < m_widgetRect.right() && pt.x() >= m_widgetRect.right() - borderWidth
                        && pt.y() >= m_widgetRect.top() && pt.y() < m_widgetRect.top() + borderWidth)
                        *result = HTTOPRIGHT;
                    // right bottom corner
                    if (pt.x() < m_widgetRect.right() && pt.x() >= m_widgetRect.right() - borderWidth
                        && pt.y() < m_widgetRect.bottom() && pt.y() >= m_widgetRect.bottom() - borderWidth)
                        *result = HTBOTTOMRIGHT;
                    // left bottom corner
                    if (pt.x() >= m_widgetRect.left() && pt.x() < m_widgetRect.left() + borderWidth
                        && pt.y() < m_widgetRect.bottom() && pt.y() >= m_widgetRect.bottom() - borderWidth)
                        *result = HTBOTTOMLEFT;
                }
                if (0 != *result)
                    return true;
            }
            for (auto bar : m_titlebars) {
                if (bar->isActiveWindow() && bar->inCaption(pt)) {
                    *result = HTCAPTION;
                    return true;
                }
            }
            return false;
        } break;
        default:
            break;
        }
    }
    return false;
}
