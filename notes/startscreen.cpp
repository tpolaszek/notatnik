#include "startscreen.h"
#include "ui_startscreen.h"
#include "filehandling.h"
#include <QListWidgetItem>
#include <QFileInfo>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

StartScreen::StartScreen(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent), ui(new Ui::StartScreen), mainWindow(mainWindow)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);
    setWindowTitle("Notatnik");
    setWindowIcon(QIcon(":/icons/note_icon.png"));

    QWidget *titleBar = new QWidget(this);
    titleBar->setFixedHeight(35);
    titleBar->setGeometry(0, 0, width(), 35);

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(10, 0, 0, 0);

    QPushButton *closeBtn = new QPushButton();

    closeBtn->setIcon(QIcon(":/icons/close.png"));
    closeBtn->setFixedSize(35, 35);

    closeBtn->setStyleSheet(R"(
        QPushButton {
            color: white;
            border: none;
            border-radius: 0. 10px, 0, 0;
            font-size: 16px;
            padding: 4px 10px;
        }
        QPushButton:hover { background: #fc3e30; }
    )");

    titleLayout->addStretch();
    titleLayout->addWidget(closeBtn);

    ui->mainHorizontalLayout->setContentsMargins(20, 35, 10, 10);

    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

    populateRecentFilesList();
    connect(ui->pushButton, &QPushButton::clicked, this, &StartScreen::onCreateFileClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &StartScreen::onOpenFileClicked);
    connect(ui->recentFilesList, &QListWidget::itemDoubleClicked, this, &StartScreen::onRecentFileDoubleClicked);
}


StartScreen::~StartScreen()
{
    delete ui;
}

void StartScreen::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) dragPos = event->globalPosition().toPoint();
}

void StartScreen::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(pos() + event->globalPosition().toPoint() - dragPos);
        dragPos = event->globalPosition().toPoint();
    }
}

void StartScreen::populateRecentFilesList()
{
    ui->recentFilesList->clear(); // czyści liste ostatnich plików

    const QList<RecentFile> files = recentMgr.recentFiles(); // lista jest zapełniana plikami z metody "recentFiles" w klasie RecentFilesManager
    if (files.isEmpty()) {
        QListWidgetItem *empty = new QListWidgetItem("Brak ostatnich plików"); // jeśli nie ma plików dodaje do listy informacje że nie ma żadnych ostatnio otwartych plików
        empty->setFlags(empty->flags() & ~Qt::ItemIsEnabled);
        ui->recentFilesList->addItem(empty);
        return;
    }

    // Dla każdego pliku
    for (const RecentFile &rf : files) {
        QFileInfo info(rf.path); // pobiera informacje z pliku
        QString label = info.fileName() + "\n" + rf.lastModified.toString("dd.MM.yyyy  hh:mm"); // dodaje date ostatniej modyfikacji do każdego wpisu na liście

        QListWidgetItem *item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, rf.path);
        item->setToolTip(rf.path);
        ui->recentFilesList->addItem(item);
    }
}


void StartScreen::onCreateFileClicked(){
    this->hide(); // po kliknięciu ukrywa okno początkowe
    mainWindow->show(); // pokzauje mainwindow
}

void StartScreen::onOpenFileClicked(){
    QString filePath = FileHandling::getOpenFilePath(this);
    if (!filePath.isEmpty()) {
        this->hide();
        mainWindow->show();
        mainWindow->openFileFromPath(filePath);
    }
}

void StartScreen::onRecentFileDoubleClicked(QListWidgetItem *item){
    QString path = item->data(Qt::UserRole).toString();
    if (!path.isEmpty()) {
        this->hide();
        mainWindow->show();
        mainWindow->openFileFromPath(path);
    }
}
