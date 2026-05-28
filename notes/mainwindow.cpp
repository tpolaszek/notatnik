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
    setWindowIcon(QIcon(":/icons/note_icon.png"));

    viewManager = nullptr; // dopiero inicjowanie zajdzie w innej metodzie ponieważ wymagane będzie aktualizowanie na bierząco

    tabWidget = ui->tabWidget;
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabClose);
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChange);

    auto *viewMode = new QToolButton(this);
    viewMode->setIcon(QIcon(":/icons/preview.png"));
    viewMode->setToolTip("Podgląd / Edycja");

    // Ustawia wielkość przycisku i ikony w nim zawartej
    viewMode->setIconSize(QSize(32, 32));
    viewMode->setFixedSize(QSize(36, 36));

    // Przycisk pokazuje się w prawym górnym rogu menuBara
    menuBar()->setCornerWidget(viewMode, Qt::TopRightCorner);
    viewMode->setStyleSheet("QToolButton { border: none; border-radius: 4px; background: transparent; }" "QToolButton:hover { border-radius: 4px; }");

    bool *isPreview = new bool(false); // przechowuje stan wyświetlania
    connect(viewMode, &QToolButton::clicked, this, [this, viewMode]() {
        EditorTab *tab = currentTab();
        if (!tab) return;
        tab->isPreview = !tab->isPreview;
        if (tab->isPreview) {
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

    // 1. Tworzymy obiekt słownika i kompletera
    syntaxDict = new SyntaxDictionary(this);
    completer = new QCompleter(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);


    syntaxDict->updateLanguageForFile("default.cpp", completer);

    addTab(); // otworzenie pustej strony
}

MainWindow::~MainWindow()
{
    for(EditorTab &tab : tabs) {
        delete tab.highlighter;
        tab.highlighter = nullptr;
    }

    delete currentHighlighter;
    delete ui;
}

int MainWindow::currentTabIndex() const{
    return tabWidget->currentIndex();
}

EditorTab *MainWindow::currentTab(){
    int ind = currentTabIndex();
    if(ind < 0 || ind >= tabs.size()) return nullptr;
    return &tabs[ind];
}

int MainWindow::addTab(const QString &filePath){
    // budowanie edytora do każdej karty moim zdaniem lepsze rozwiązanie niż tworzenie .ui
    auto *container = new QWidget(); // tworzenie nowego obiektu QWidget
    auto *hLayout = new QHBoxLayout(); // tworzenie nowego QHBoxLayout

    auto *lineCounter = new QPlainTextEdit(container);
    lineCounter->setReadOnly(true);
    lineCounter->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lineCounter->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lineCounter->setObjectName("lineCounter");

    auto *editor = new QPlainTextEdit(container);
    editor->setObjectName("noteText");

    hLayout->addWidget(lineCounter);
    hLayout->addWidget(editor);

    EditorTab tab;
    tab.editor = editor;
    tab.lineCounter = lineCounter;
    tab.filePath = filePath;

    QString label = filePath.isEmpty() ? "Nowy plik" : QFileInfo(filePath).fileName();

    int ind = tabWidget->addTab(container, label);
    tabs.insert(ind, tab);

    TextTools::setupLineCounterUI(editor, lineCounter);
    ViewManager::setupBottomSpace(editor);

    connectEditorSignals(ind);
    tabWidget->setCurrentIndex(ind);

    return ind;
}
void MainWindow::connectEditorSignals(int tabIndex){
    if(tabIndex < 0 || tabIndex >= tabs.size()) return;
    EditorTab &tab = tabs[tabIndex];

    //Aktualizowanie licznika wierszy
    connect(tab.editor, &QPlainTextEdit::textChanged, this, [this, tabIndex](){
        if(tabIndex>= tabs.size()) return;
        EditorTab &t = tabs[tabIndex];
        TextTools::updateLineCounter(t.editor, t.lineCounter);

        // Autozupełnianie
        if(syntaxDict && completer) syntaxDict->handleTextChange(t.editor, completer);
    });

    connect(tab.editor->document(), &QTextDocument::modificationChanged, this, [this, tabIndex](bool modified){
        if(tabIndex >= tabs.size()) return;
        updateTabTitle(tabIndex);

        if(tabIndex == currentTabIndex()){
            const EditorTab &t = tabs[tabIndex];
            QString base = t.filePath.isEmpty() ? "Nowy plik" : QFileInfo(t.filePath).fileName();
            setTitle((modified ? "* " : " ") + base);
        }
    });
}

bool MainWindow::closeTab(int index){
    if (index < 0 || index >= tabs.size()) return false;

    if (!proceedWithSafetyCheck(index)) return false;

    delete tabs[index].highlighter;
    tabs[index].highlighter = nullptr;

    tabs.removeAt(index);
    tabWidget->removeTab(index);

    if(tabs.isEmpty()) addTab();

    return true;
}

void MainWindow::updateTabTitle(int tabIndex){
    if(tabIndex < 0 || tabIndex >= tabs.size()) return;
    const EditorTab &tab = tabs[tabIndex];

    QString name = tab.filePath.isEmpty() ? "Nowy plik" : QFileInfo(tab.filePath).fileName();

    if(tab.editor->document()->isModified()) name.prepend("* "); // jeśli plik nie jest zapisany to dodaje gwiazdke przed nazwą

    tabWidget->setTabText(tabIndex, name);
    tabWidget->setTabToolTip(tabIndex, tab.filePath);
}

void MainWindow::onTabClose(int index){
    closeTab(index);
}

void MainWindow::onTabChange(int index){
    if(index < 0 || index >= tabs.size()) return;
    EditorTab &tab = tabs[index];

    delete viewManager;
    viewManager = new ViewManager(tab.editor, tab.lineCounter, this->statusBar());
    viewManager->setupEditorVisuals(tab.editor);
    completer->setWidget(tab.editor);
    completer->popup()->installEventFilter(viewManager);
    viewManager->setCompleter(completer);

    QString base = tab.filePath.isEmpty() ? "Nowy plik" : QFileInfo(tab.filePath).fileName();

    bool mod = tab.editor->document()->isModified();
    setTitle((mod ? "* " : "") + base);

    viewManager->applyFontSize(settingsManager->fontSize());
}

// Zmienia tytuł okna na nazwe pliku + Notatnik
void MainWindow::setTitle(QString title){
    if(title.isEmpty()){
        setWindowTitle("Notatnik");
        return;
    };
    this->setWindowTitle(title + " - Notatnik");
}

//Aplikuje podkreślenie składni
void MainWindow::applyHighlighter(int tabIndex, const QString& filePath)
{
    if(tabIndex < 0 || tabIndex>=tabs.size()) return;
    EditorTab &tab = tabs[tabIndex];

    delete tab.highlighter;
    tab.highlighter = nullptr;

    if (filePath.isEmpty()) return;

    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();
    QJsonObject grammar = grammarLoader.grammarForExtension(ext);

    if(!grammar.isEmpty()) {
        tab.highlighter = new SyntaxHighlighter(tab.editor->document());
        tab.highlighter->loadFromJson(grammar);
    }
}

// Otwiera plik
void MainWindow::openFileFromPath(const QString &filePath){
    if(filePath.isEmpty()) return; // jeśli nie ma tego pliku nie otwieraj go

    for(int i = 0; i<tabs.size(); ++i){
        if(tabs[i].filePath == filePath){
            tabWidget->setCurrentIndex(i);
            return;
        }
    }

    int targetIndex = currentTabIndex();
    EditorTab *cur = currentTab();
    bool reuseCurrentTab = cur && cur->filePath.isEmpty() && !cur->editor->document()->isModified();

    if(!reuseCurrentTab){
        targetIndex = addTab();
    }

    QString content = FileHandling::openFile(filePath); // przechowywuje zawartość pliku
    if(content.isNull()) return;

    EditorTab &tab = tabs[targetIndex];
    tab.filePath = filePath;
    tab.editor->setPlainText(content); // przekazuje tekst do dokumentu
    tab.editor->document()->setModified(false); // domyślnie ustawia plik na nieedytowany

    updateTabTitle(targetIndex); // Ustawia tytuł karty
    setTitle(filePath); // ustawia nazwe w metodzie setTitle
    applyHighlighter(targetIndex, filePath); // ustaiwa odpowiednie podkreślenie do rodzaju pliku
    recordAndRefresh(filePath); // zapisuje plik do ostatnio otwartych

    if (syntaxDict && completer) {
        syntaxDict->updateLanguageForFile(filePath, completer);
    }
}

// Metoda zapisuje i odświeża ostatnio otwarte pliki
void MainWindow::recordAndRefresh(const QString &filePath) {
    recentMgr.recordFile(filePath);
}

// Metoda zapełnia menu z ostatnimi plikami
void MainWindow::fillRecentMenu(){
    recentMenu->clear(); // czyści menu

    const QList<RecentFile> files = recentMgr.recentFiles(); // dodaje do listy ostatnie pliki

    // Jeśli nie ma plików
    if(files.isEmpty()) {
        QAction *empty = recentMenu->addAction("Brak plików"); // dodaje do menu informacje o braku plików
        empty->setEnabled(false);
        return;
    }

    // Dla każdego ostatnio otwartego pliku
    for(const RecentFile &rf : files){
        QFileInfo info(rf.path); // pobieramy informacje o pliku

        QString label = info.fileName() + "    " + rf.lastModified.toString("dd.MM.yyyy  hh:mm");

        QAction *act = recentMenu->addAction(label);
        act->setToolTip(rf.path);

        const QString path = rf.path;
        connect(act, &QAction::triggered, this, [this, path](){
            openFileFromPath(path);
        });
    }
}

void MainWindow::on_openFile_triggered() {
    QString filePath = FileHandling::getOpenFilePath(this);
    if(filePath.isEmpty()) return;
    openFileFromPath(filePath);
}

void MainWindow::on_createFile_triggered() {
    EditorTab *tab = currentTab();
    if(!tab) return;

    tab->editor->clear(); // czyści widok
    currentFilePath = QString();
    this->setWindowTitle("Notatnik"); // ustawia nazwe okna na "Notatnik"
}

void MainWindow::on_saveFile_triggered() {
    EditorTab *tab = currentTab();
    if(!tab) return;

    if (tab->filePath.isEmpty()) {
        on_saveFileAs_triggered();
    } else {
        if (FileHandling::saveFile(tab->filePath, tab->editor->toPlainText())) {
            tab->editor->document()->setModified(false);
            updateTabTitle(currentTabIndex());
        }
    }
}

void MainWindow::on_saveFileAs_triggered() {
    EditorTab *tab = currentTab();
    if(!tab) return;

    QString fileName = FileHandling::getSaveFilePath(this);
    if (fileName.isEmpty()) return;
    tab->filePath = fileName;

    if (FileHandling::saveFile(fileName, tab->editor->toPlainText())) {
        tab->editor->document()->setModified(false);
        updateTabTitle(currentTabIndex());
        setTitle(QFileInfo(fileName).fileName());
        applyHighlighter(currentTabIndex(), fileName);
        recordAndRefresh(fileName);

        if (syntaxDict && completer) {
            syntaxDict->updateLanguageForFile(currentFilePath, completer);
        }
    }
}


void MainWindow::on_Find_triggered() {
    EditorTab *tab = currentTab();
    if (!tab) return;

    bool ok;
    int counter = 0;
    do {
        int totalMatches = TextTools::getCount(tab->editor, lastSearchTerm); // pobiera ilość wszystkich wyników
        QString count = "(" + QString::number(counter) + "/" + QString::number(totalMatches) + ")";

        QString searchTerm = QInputDialog::getText(this, "Szukaj " + count, "Znajdź:", QLineEdit::Normal, lastSearchTerm, &ok); // otwiera okno z inputem które pobiera słowo do szukania

        // Jeśli użytkownik kliknął OK i tekst nie jest pusty
        if (ok && !searchTerm.isEmpty()) {
            counter++;
            lastSearchTerm = searchTerm;

            // Wywołujemy metode szukania
            TextTools::findText(this, tab->editor, lastSearchTerm);

            // Wymuszanie odświeżenia całego okna
            tab->editor->repaint();
        }
        if(counter > totalMatches) counter = 1;
    } while (ok && tab->editor->textCursor().hasSelection());
    // Pętla działa tak długo, póki nie klikniesz anuluj i program znajduje kolejne słowa
}

// Metoda otwiera okno ustawień
void MainWindow::on_settings_triggered() {
    SettingsDialog dlg(settingsManager, viewManager, this);
    dlg.exec();
}

void MainWindow::on_FindAndReplace_triggered() {
    EditorTab *tab = currentTab();
    if (!tab) return;

    TextTools::findAndReplace(this, tab->editor);
}

void MainWindow::on_closeFile_triggered()
{
    closeTab(currentTabIndex());
}
void MainWindow::closeEvent(QCloseEvent *event) {
    for (int i = tabs.size() - 1; i >= 0; --i) {
        tabWidget->setCurrentIndex(i);
        if (!proceedWithSafetyCheck(i)) {
            event->ignore();
            return;
        }
    }
    event->accept();
}
bool MainWindow::proceedWithSafetyCheck(int tabIndex) {
    if(tabIndex< 0 || tabIndex >= tabs.size()) return true;
    EditorTab &tab = tabs[tabIndex];

    // 1. Pytamy "robota od plików", co sądzi o sytuacji
    FileHandling::SaveResult result = FileHandling::checkSaveStatus(this, tab.editor);

    // 2. Jeśli użytkownik chce zapisać, odpalamy Twoją gotową funkcję
    if (result == FileHandling::SaveResult::SaveRequested) {
        tabWidget->setCurrentIndex(tabIndex);
        on_saveFile_triggered();
        // Jeśli po zapisie nadal jest "modified" (bo np. zamknął okno Save As), przerywamy
        return !tab.editor->document()->isModified();
    }

    // 3. Jeśli Cancel - zwracamy false (nie idź dalej). Jeśli Discard - true (idź dalej).
    return (result != FileHandling::SaveResult::CancelAction);
}
