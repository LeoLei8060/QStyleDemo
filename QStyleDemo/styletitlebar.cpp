#include "styletitlebar.h"
#include "3rdparty/qutils/FontManager.h"
#include "fontStyleDefines.h"
#include "ui_styletitlebar.h"

StyleTitlebar::StyleTitlebar(QWidget *parent)
    : FramelessTitle(parent)
    , ui(new Ui::StyleTitlebar)
{
    ui->setupUi(this);
    initUi();
}

StyleTitlebar::~StyleTitlebar()
{
    delete ui;
}

bool StyleTitlebar::inCaption(const QPoint &pt)
{
    const qreal dpi = qApp->devicePixelRatio();
    int         left1 = ui->iconLabel->geometry().right() * dpi;
    int         right1 = ui->linkLabel->geometry().left() * dpi;
    QRect       rc1(left1, 0, right1 - left1, height() * dpi);
    int         left2 = ui->linkLabel->geometry().right() * dpi;
    int         right2 = ui->minBtn->geometry().left() * dpi;
    QRect       rc2(left2, 0, right2 - left2, height() * dpi);
    if (rc1.contains(pt) || rc2.contains(pt))
        return true;
    return false;
}

void StyleTitlebar::initUi()
{
    ui->titleLabel->setText("Qt CSS样式Demo（作者：LeoLei 博客：");
    ui->linkLabel->setText("<a href=\"https://leolei8060.blog.csdn.net\">LeoLei</a>）");

    auto font = qutils::FontManager::instance()->fontAt(FontStyleE::kIcon_style);
    ui->minBtn->setFont(font);
    ui->minBtn->setText(QChar(0xe629));
    ui->minBtn->setToolTip("最小化");

    ui->maxBtn->setFont(font);
    ui->maxBtn->setText(QChar(0xe60e));
    ui->maxBtn->setToolTip("最大化");

    ui->closeBtn->setFont(font);
    ui->closeBtn->setText(QChar(0xe61e));
    ui->closeBtn->setToolTip("退出");

    initConnect();
}

void StyleTitlebar::initConnect()
{
    connect(ui->minBtn, &QPushButton::clicked, this, [this]() { parentWidget()->showMinimized(); });
    connect(ui->maxBtn, &QPushButton::clicked, this, [this]() {
        parentWidget()->isMaximized() ? parentWidget()->showNormal() : parentWidget()->showMaximized();
    });
    connect(ui->closeBtn, &QPushButton::clicked, this, [this]() { qApp->quit(); });
}

void StyleTitlebar::onMaximized()
{
    ui->maxBtn->setText(QChar(0xe613));
    ui->maxBtn->setToolTip("还原");
}

void StyleTitlebar::onNormaled()
{
    ui->maxBtn->setText(QChar(0xe60e));
    ui->maxBtn->setToolTip("最大化");
}
