#ifndef STYLETITLEBAR_H
#define STYLETITLEBAR_H

#include "3rdparty/Frameless/Frameless.h"

#include <QWidget>

namespace Ui {
class StyleTitlebar;
}

class StyleTitlebar : public FramelessTitle {
    Q_OBJECT

public:
    explicit StyleTitlebar(QWidget* parent = nullptr);
    ~StyleTitlebar();

    virtual bool inCaption(const QPoint& pt) override;

    void initUi();

    void initConnect();

public slots:
    virtual void onMaximized() override;
    virtual void onNormaled() override;

private:
    Ui::StyleTitlebar* ui;
};

#endif // STYLETITLEBAR_H
