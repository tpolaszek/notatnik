#include "startscreen.h"
#include "ui_startscreen.h"
#include "filehandling.h"
#include <QListWidgetItem>
#include <QFileInfo>

StartScreen::StartScreen(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent), ui(new Ui::StartScreen), mainWindow(mainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Notatnik");

    populateRecentFilesList();

    connect(ui->pushButton,  &QPushButton::clicked, this, &StartScreen::onCreateFileClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &StartScreen::onOpenFileClicked);
    connect(ui->recentFilesList, &QListWidget::itemDoubleClicked, this, &StartScreen::onRecentFileDoubleClicked);
}

StartScreen::~StartScreen()
{
    delete ui;
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
