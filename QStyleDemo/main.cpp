#include "3rdparty/qutils/FontManager.h"
#include "fontStyleDefines.h"
#include "widget.h"

#include <QApplication>
#include <QMessageBox>

#define QWARNING(A) QMessageBox::warning(nullptr, "警告", A, "确定")

bool initUIStyle(QApplication* a)
{
    QFile file("://res/qss/black.css");
    QString styleSheet = "";
    if (file.open(QFile::ReadOnly)) {
        styleSheet = QLatin1String(file.readAll());

        a->setStyleSheet(styleSheet);
        file.close();
        return true;
    }
    return false;
}

bool initFont()
{
    return qutils::FontManager::instance()->addThirdpartyFont("://res/font/iconfont.ttf", FontStyleE::kIcon_style);
}

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    if (!initUIStyle(&a))
        QWARNING("black.css样式文件加载失败！");

    if (!initFont())
        QWARNING("加载ICON字体库失败！");

    a.installNativeEventFilter(FrameNativeEventFilter::instance());

    Widget w;
    w.show();
    return a.exec();
}
