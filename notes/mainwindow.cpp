#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsmanager.h"
#include "texttools.h"
#include "settingsdialog.h"
#include <QFileDialog>
#include <QTextStream>
#include <QFileInfo>
#include <QStandardPaths>
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentHighlighter(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle("Notatnik");

    TextTools::setupLineCounterUI(ui->noteText, ui->lineCounter);
    connect(ui->noteText, &QPlainTextEdit::textChanged, this, [this]() {
        TextTools::updateLineCounter(ui->noteText, ui->lineCounter);
    });

    grammarLoader.loadFromDirectory(":/syntax");

    settingsManager = new SettingsManager(this);
    settingsManager->applyToApp();

    recentMenu = ui->menuOstatnioOtwarte;
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::fillRecentMenu);
}

MainWindow::~MainWindow()
{
    delete currentHighlighter;
    delete ui;
}

void MainWindow::setTitle(QString title){
    if(title.isEmpty()) return;
    this->setWindowTitle(title + " - Notatnik");
}

void MainWindow::applyHighlighter(const QString& filePath)
{
    delete currentHighlighter;
    currentHighlighter = nullptr;

    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();

    QJsonObject grammar = grammarLoader.grammarForExtension(ext);
    if(!grammar.isEmpty()){
        currentHighlighter = new SyntaxHighlighter(ui->noteText->document());
        currentHighlighter->loadFromJson(grammar);
    }
}

void MainWindow::openFileFromPath(const QString &filePath){
    if(filePath.isEmpty()) return;

    QString content = FileHandling::openFile(filePath);
    if(content.isNull()) return;

    ui->noteText->setPlainText(content);
    currentFilePath = filePath;
    setTitle(filePath);
    applyHighlighter(filePath);
    recordAndRefresh(filePath);
}

void MainWindow::recordAndRefresh(const QString &filePath) {
    recentMgr.recordFile(filePath);  // dot not arrow, it's a value now
}

void MainWindow::fillRecentMenu(){
    recentMenu->clear();

    const QList<RecentFile> files = recentMgr.recentFiles();
    if(files.isEmpty()) {
        QAction *empty = recentMenu->addAction("Brak plików");
        empty->setEnabled(false);
        return;
    }

    for(const RecentFile &rf : files){
        QFileInfo info(rf.path);

        QString label = info.fileName() + "    " + rf.lastModified.toString("dd.MM.yyyy  hh:mm");

        QAction *act = recentMenu->addAction(label);
        act->setToolTip(rf.path);

        const QString path = rf.path;
        connect(act, &QAction::triggered, this, [this, path](){
            openFileFromPath(path);
        });
    }
}

void MainWindow::on_openFile_triggered()
{
    QString filePath = FileHandling::getOpenFilePath(this);
    if(filePath.isEmpty()) return;

    QString content = FileHandling::openFile(filePath);
    if(!content.isNull()){
        ui->noteText->setPlainText(content);
        currentFilePath = filePath;
        setTitle(filePath);
        applyHighlighter(filePath);
        recordAndRefresh(filePath);
    }
}

void MainWindow::on_createFile_triggered()
{
    delete currentHighlighter;
    currentHighlighter = nullptr;

    ui->noteText->clear();
    currentFilePath = QString();
    this->setWindowTitle("Notatnik");
}

void MainWindow::on_saveFile_triggered()
{
    if (currentFilePath.isEmpty()) {
        on_saveFileAs_triggered();
    } else {
        FileHandling::saveFile(currentFilePath, ui->noteText->toPlainText());
    }
}

void MainWindow::on_saveFileAs_triggered()
{
    QString fileName = FileHandling::getSaveFilePath(this);
    if (fileName.isEmpty()) return;
    currentFilePath = fileName;

    if (FileHandling::saveFile(fileName, ui->noteText->toPlainText())) {
        setTitle(currentFilePath);
        applyHighlighter(currentFilePath);
        recordAndRefresh(currentFilePath);
    }
}

void MainWindow::on_actionFind_triggered()
{
    bool isConfirmed;

    // Pobieramy tekst od użytkownika
    QString searchTerm = QInputDialog::getText(this, "Search", "Find what:", QLineEdit::Normal, lastSearchTerm, &isConfirmed);

    // Sprawdzamy, czy użytkownik zatwierdził (OK) i czy wpisał cokolwiek
    if (isConfirmed && !searchTerm.isEmpty()) {
        TextTools::findText(this, ui->noteText, searchTerm);
    }
}



void MainWindow::on_settings_triggered()
{
    SettingsDialog dlg(settingsManager, this);
    dlg.exec();
}

void MainWindow::on_actionFindAndReplace_triggered()
{
    TextTools::findAndReplace(this, ui->noteText);
}



void MainWindow::on_closeFile_triggered()
{
    delete currentHighlighter;
    currentHighlighter = nullptr;

    ui->noteText->clear();
    currentFilePath = QString();
    this->setWindowTitle("Notatnik");
}

