#ifndef WIDGET_H
#define WIDGET_H

#include "3rdparty/Frameless/Frameless.h"
#include "styletitlebar.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public Frameless {
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();

    void initUi();

    void initConnect();

private:
    Ui::Widget* ui;
};
#endif // WIDGET_H
