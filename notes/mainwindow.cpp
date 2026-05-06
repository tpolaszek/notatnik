#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsmanager.h"
#include "texttools.h"
#include "settingsdialog.h"
#include "viewmanager.h"
#include <QFileDialog>
#include <QTextStream>
#include <QFileInfo>
#include <QStandardPaths>
#include <QInputDialog>
#include <QToolButton>


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

    auto *viewMode = new QToolButton(this);
    viewMode->setIcon(QIcon(":/icons/preview.png"));
    viewMode->setToolTip("Podgląd / Edycja");

    // Ustawia wielkość przycisku i ikony w nim zawartej
    viewMode->setIconSize(QSize(32, 32));
    viewMode->setFixedSize(QSize(32, 32));

    // Przycisk pokazuje się w prawym górnym rogu menuBara
    menuBar()->setCornerWidget(viewMode, Qt::TopRightCorner);

    // Ustawienie przezroczystego tła dla przycisku
    viewMode->setStyleSheet("QToolButton { border: none; background: transparent; }" "QToolButton:hover { background: rgba(255,255,255,30); border-radius: 4px; }");

    bool *isPreview = new bool(false); // przechowuje stan wyświetlania
    connect(viewMode, &QToolButton::clicked, this, [this, viewMode, isPreview]() {
        *isPreview = !*isPreview;
        if (*isPreview) {
            viewManager->switchToPreview();
            viewMode->setIcon(QIcon(":/icons/edit.png"));
        } else {
            viewManager->switchToEdit();
            viewMode->setIcon(QIcon(":/icons/preview.png"));
        }
    });

    grammarLoader.loadFromDirectory(":/syntax/syntax");

    settingsManager = new SettingsManager(this);
    settingsManager->applyToApp();

    recentMenu = ui->menuOstatnioOtwarte;
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::fillRecentMenu);

    viewManager = new ViewManager(ui->noteText, ui->lineCounter, this->statusBar());

    // Tabulacja te 2 linijki (reszta znajduje się w viewManager)
    ui->noteText->installEventFilter(this);
    viewManager->setupEditorVisuals(ui->noteText);

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
    recentMgr.recordFile(filePath);
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


void MainWindow::on_Find_triggered()
{
    bool ok;
    // 1. Pobierasz tekst (podpowiada ostatnio szukane słowo)
    QString searchTerm = QInputDialog::getText(this, "Szukaj", "Znajdź:", QLineEdit::Normal, lastSearchTerm, &ok);

    // 2. Jeśli kliknął OK i coś wpisał
    if (ok && !searchTerm.isEmpty()) {
        lastSearchTerm = searchTerm; // Aktualizujesz zmienną w MainWindow

        // 3. Wywołujesz swoją funkcję z TextTools
        // Każde kliknięcie "szukaj" i Enter teraz znajdzie następną "sigmę"
        TextTools::findText(this, ui->noteText, lastSearchTerm);
    }
}

void MainWindow::on_settings_triggered()
{
    SettingsDialog dlg(settingsManager, this);
    dlg.exec();
}

void MainWindow::on_FindAndReplace_triggered()
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
