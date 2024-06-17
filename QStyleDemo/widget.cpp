#include "widget.h"
#include "./ui_widget.h"
#include "3rdparty/qutils/FontManager.h"
#include "fontStyleDefines.h"

#include <QDirModel>
#include <QScrollBar>
#include <QTextCodec>

Widget::Widget(QWidget *parent)
    : Frameless(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    FrameNativeEventFilter::instance()->addTitlebar(ui->titlebar);
    this->setTitlebar(ui->titlebar);

    auto i = ui->stackedWidget->count();

    initUi();
    ui->TabBtn1->click();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initUi()
{
    setWindowIcon(QIcon(":/res/image/myIcon.ico"));

    QDirModel *model = new QDirModel(this);
    ui->treeView->setModel(model);
    ui->tableView->setModel(model);
    ui->listView->setModel(model);

    ui->treeView->header()->setOffsetToLastSection();
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setFocusPolicy(Qt::FocusPolicy::NoFocus); // 去掉选中时的虚线框
    ui->tableView->setSelectionModel(ui->treeView->selectionModel());
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->listView->setSelectionModel(ui->treeView->selectionModel());
    ui->listView->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    ui->treeView->verticalScrollBar()->setContentsMargins(0, 30, 0, 0);

    auto comboxView = new QListView(ui->comboBox);
    comboxView->setFocusPolicy(Qt::FocusPolicy::NoFocus); // 没法去掉虚线框，在CSS中用outline去掉
    ui->comboBox->setView(comboxView);
    ui->comboBox->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    auto font = qutils::FontManager::instance()->fontAt(FontStyleE::kIcon_style);
    font.setPixelSize(16);
    ui->addBtn->setFont(font);
    ui->addBtn->setText(QChar(0xe604));
    ui->addBtn->setToolTip("添加下拉项");

    // text
    QFile file("://res/text.txt");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QByteArray  ba = file.readAll();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString     str = codec->toUnicode(ba);
        ui->textEdit->setText(str);
        ui->plainTextEdit->setPlainText(str);
        file.close();
    }

    initConnect();
}

void Widget::initConnect()
{
    static int gComboxNum = 3;
    connect(ui->addBtn, &QPushButton::clicked, this, [this]() {
        ui->comboBox->addItem(QString("下拉数据%1").arg(++gComboxNum));
    });

    auto tabChanged = [this](int index) {
        ui->stackedWidget->setCurrentIndex(index);
        ui->TabBtn1->setChecked(index == 0 ? true : false);
        ui->TabBtn2->setChecked(index == 0 ? false : true);
    };
    connect(ui->TabBtn1, &QPushButton::clicked, this, [=]() { tabChanged(0); });
    connect(ui->TabBtn2, &QPushButton::clicked, this, [=]() { tabChanged(1); });

    connect(ui->horizontalSlider, &QSlider::valueChanged, ui->progressBar, [=](int value) {
        ui->progressBar->setValue(value);
    });
}
