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
#include <QIcon>

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
    // Tą ikonę trzeba zmienić na coś lepszego
    setWindowIcon(QIcon(":/icons/note_icon.png"));

    auto *viewMode = new QToolButton(this);
    viewMode->setIcon(QIcon(":/icons/preview.png"));
    viewMode->setToolTip("Podgląd / Edycja");

    // Ustawia wielkość przycisku i ikony w nim zawartej
    viewMode->setIconSize(QSize(32, 32));
    viewMode->setFixedSize(QSize(36, 36));

    // Przycisk pokazuje się w prawym górnym rogu menuBara
    menuBar()->setCornerWidget(viewMode, Qt::TopRightCorner);

    // Ustawienie szaregp tła dla przycisku, dlatego że czarne ikony zlewają się z tłem
    viewMode->setStyleSheet("QToolButton { border: none; border-radius: 4px; background: #858585; }" "QToolButton:hover { background: #696868; border-radius: 4px; }");

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
    viewManager->setupEditorVisuals(ui->noteText);

    // 1. Tworzymy obiekt słownika i kompletera
    syntaxDict = new SyntaxDictionary(this);
    completer = new QCompleter(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWidget(ui->noteText);

    // ====================================================================
    // <<< TUTAJ JEST TA KLUCZOWA DOKŁADKA >>>
    // Podpinamy nasz ViewManager jako filtr zdarzeń bezpośrednio pod okienko podpowiedzi!
    // Dzięki temu każdy kliknięty TAB/Enter w dymku trafi prosto do naszego eventFilter.
    completer->popup()->installEventFilter(viewManager);
    // ====================================================================

    viewManager->setCompleter(completer);

    // <<< TO DOPISZ TUTAJ >>>
    connect(completer, QOverload<const QString &>::of(&QCompleter::activated), this, [this](const QString &completion) {
        // Pobieramy prefix, który użytkownik zdążył wpisać (np. "pr")
        QString prefix = completer->completionPrefix();

        // Obliczamy ile liter brakuje (np. dla "print" i "pr" -> brakuje "int")
        QString toInsert = completion.mid(prefix.length());

        // Wklejamy brakujący tekst do edytora
        ui->noteText->insertPlainText(toInsert);
    });

    // 2. ŁĄCZYMY WSZYSTKO W JEDNYM MIEJSCU (Licznik linii + Słownik)
    connect(ui->noteText, &QPlainTextEdit::textChanged, this, [this]() {
        // Aktualizacja licznika linii
        TextTools::updateLineCounter(ui->noteText, ui->lineCounter);

        // Wywołanie słownika
        if (syntaxDict && completer) {
            syntaxDict->handleTextChange(ui->noteText, completer);
        }
    });

    // 3. STARTOWY SŁOWNIK: Żeby działało od razu po włączeniu notatnika
    syntaxDict->updateLanguageForFile("default.cpp", completer);


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

    if (syntaxDict && completer) {
        syntaxDict->updateLanguageForFile(filePath, completer);
    }
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
        // POPRAWKA: Zamiast fileName przekazujemy filePath
        if (syntaxDict && completer) {
            syntaxDict->updateLanguageForFile(filePath, completer);
        }
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
        if (FileHandling::saveFile(currentFilePath, ui->noteText->toPlainText())) {
            // Resetujemy flagę - to mówi programowi, że zmiany zostały zapisane
            ui->noteText->document()->setModified(false);
        }
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

        if (syntaxDict && completer) {
            syntaxDict->updateLanguageForFile(currentFilePath, completer);
        }
        // Resetujemy flagę po zapisaniu nowego pliku
        ui->noteText->document()->setModified(false);
    }
}


void MainWindow::on_Find_triggered()
{

    bool ok;
    int counter = 0;
    do {
        int totalMatches = TextTools::getCount(ui->noteText, lastSearchTerm);
        QString count = "(" + QString::number(counter) + "/" + QString::number(totalMatches) + ")";

        // 1. Pokazujemy Twoje standardowe okienko
        QString searchTerm = QInputDialog::getText(this, "Szukaj " + count, "Znajdź:", QLineEdit::Normal, lastSearchTerm, &ok);

        // 2. Jeśli użytkownik kliknął OK i tekst nie jest pusty
        if (ok && !searchTerm.isEmpty()) {
            counter++;
            lastSearchTerm = searchTerm;

            // 3. Wywołujemy Twoje szukanie z TextTools
            TextTools::findText(this, ui->noteText, lastSearchTerm);

            // Wymuszamy, aby Qt odświeżyło edytor i pokazało podświetlone słowo zanim otworzy kolejne okno
            ui->noteText->repaint();
        }
        if(counter > totalMatches) counter = 1;
    } while (ok && ui->noteText->textCursor().hasSelection());
    // Pętla działa tak długo, jak klikasz OK i program znajduje kolejne słowa
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
void MainWindow::closeEvent(QCloseEvent *event) {
    // Wywołujemy Twoją funkcję sprawdzającą
    if (proceedWithSafetyCheck()) {
        event->accept(); // Pozwól na zamknięcie okna
    } else {
        event->ignore(); // Zatrzymaj zamykanie okna (użytkownik kliknął Cancel)
    }
}
bool MainWindow::proceedWithSafetyCheck() {
    // 1. Pytamy "robota od plików", co sądzi o sytuacji
    FileHandling::SaveResult result = FileHandling::checkSaveStatus(this, ui->noteText);

    // 2. Jeśli użytkownik chce zapisać, odpalamy Twoją gotową funkcję
    if (result == FileHandling::SaveResult::SaveRequested) {
        on_saveFile_triggered();
        // Jeśli po zapisie nadal jest "modified" (bo np. zamknął okno Save As), przerywamy
        return !ui->noteText->document()->isModified();
    }

    // 3. Jeśli Cancel - zwracamy false (nie idź dalej). Jeśli Discard - true (idź dalej).
    return (result != FileHandling::SaveResult::CancelAction);
}
