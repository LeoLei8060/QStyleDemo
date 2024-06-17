#ifndef FRAMEEVENTFILTER_H
#define FRAMEEVENTFILTER_H

#include <QAbstractNativeEventFilter>
#include <QDebug>
#include <QEvent>
#include <QObject>

#include <windows.h>

class Frameless;
class FramelessTitle;

class FrameEventFilter : public QObject {
    Q_OBJECT
public:
    static FrameEventFilter* instance()
    {
        static FrameEventFilter instance;
        return &instance;
    }

    bool eventFilter(QObject* watched, QEvent* event) override;

    void addResizeWidget(Frameless* widget) { m_resizeWidgets.append(widget); }

    void removeResizeWidget(Frameless* widget) { m_resizeWidgets.removeOne(widget); }

private:
    explicit FrameEventFilter(QObject* parent = nullptr);
    QList<Frameless*> m_resizeWidgets;
};

class FrameNativeEventFilter : public QAbstractNativeEventFilter {
public:
    static FrameNativeEventFilter* instance();
    FrameNativeEventFilter() { }
    virtual ~FrameNativeEventFilter() { }

    virtual bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

    void addTitlebar(FramelessTitle* bar) { m_titlebars.append(bar); }
    void removeTitlebar(FramelessTitle* bar) { m_titlebars.removeOne(bar); }

    void addWindowID(HWND winId) { m_windows.append(winId); }
    void removeWindowID(HWND winId) { m_windows.removeOne(winId); }
    bool isExist_winId(HWND winId)
    {
        for (auto id : m_windows) {
            if (id == winId)
                return true;
        }
        return false;
    }

private:
    QList<FramelessTitle*> m_titlebars;
    QList<HWND> m_windows;
};

#endif // FRAMEEVENTFILTER_H
