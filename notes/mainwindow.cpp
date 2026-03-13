#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QScrollBar>
#include <QInputDialog>
#include <QMessageBox>
#include <grammarloader.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Notatnik");
    ustawLicznikWierszy();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setTitle(QString title){
    // Obsługa błędów
    if(title.isEmpty()) return;

    // Ustawienie nazwy okna na lokalizacje pliku np: "C:/Users/qwerty/notatka.txt - Notatnik"
    this->setWindowTitle(title + " - Notatnik");
}


void MainWindow::on_openFile_triggered(){
    QString filePath = FileHandling::getOpenFilePath(this);

    // Obsługa błędów
    if(filePath.isEmpty()) return;

    // Wyświetlanie zawartości
    QString content = FileHandling::openFile(filePath);
    if(!content.isNull()){
        ui->noteText->setPlainText(content);
        currentFilePath = filePath;
        setTitle(filePath);
    }
}

void MainWindow::on_createFile_triggered(){
    // Resetuje widok aplikacji de facto plik będzie tworzony przy zapisie pliku
    ui->noteText->clear();
    currentFilePath = QString();
    this->setWindowTitle("Notatnik");
}

// Otwieramy okno systemowe tylko jeśli nie zostało wcześniej nazwane
void MainWindow::on_saveFile_triggered()
{
    // Jeśli plik nie ma nazwy to wywołujemy funkcję zapisz jako
    if(currentFilePath.isEmpty()) {
        on_saveFileAs_triggered();
    } else {
        QFile file(currentFilePath);

        if(file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << ui->noteText->toPlainText();
            file.close();
        }
    }
}

// Zawsze otwieramy okno systemowe
void MainWindow::on_saveFileAs_triggered()
{

    // Funkcja która otwiera okno systemowe
    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz jako", "", "Pliki tekstowe (*.txt);;Wszystkie pliki (*)");

    // Zabezpieczenie przed wciśnięciem anuluj
    if(!fileName.isEmpty()) {
        currentFilePath = fileName;

        // Tworzenie ficzynie pliku na dysku
        QFile file(fileName);

        if(file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << ui->noteText->toPlainText();
            setTitle(currentFilePath);
            file.close();
        }

    }
}

void MainWindow::ustawLicznikWierszy()
{
    // Konfiguracja panelu
    ui->licznikWierszy->setReadOnly(true);
    ui->licznikWierszy->setFocusPolicy(Qt::NoFocus);
    ui->licznikWierszy->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->licznikWierszy->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Połączenie: przy zmianie tekstu wywołaj odświeżanie
    connect(ui->noteText, &QTextEdit::textChanged, this, [=]() {
        QString numery;
        int iloscLinii = ui->noteText->document()->blockCount();

        for (int i = 1; i <= iloscLinii; ++i) {
            numery += QString::number(i) + "\n";
        }

        ui->licznikWierszy->setPlainText(numery);

        // Wyrównanie bez podświetlenia
        QTextCursor cursor = ui->licznikWierszy->textCursor();
        cursor.select(QTextCursor::Document);
        QTextBlockFormat format;
        format.setAlignment(Qt::AlignRight);
        cursor.mergeBlockFormat(format);
        cursor.clearSelection();
        ui->licznikWierszy->setTextCursor(cursor);

        // Synchronizacja suwaka (przy pisaniu)
        ui->licznikWierszy->verticalScrollBar()->setValue(ui->noteText->verticalScrollBar()->value());
    });

    // Połączenie: przy przewijaniu
    connect(ui->noteText->verticalScrollBar(), &QScrollBar::valueChanged, this, [=](int value) {
        ui->licznikWierszy->verticalScrollBar()->setValue(value);
    });
}


void MainWindow::on_actionZnajdz_triggered()
{
    static QString word;
    bool ok = true;

    // Pętla sprawia, że okno nie znika "na zawsze" po kliknięciu OK
    while (ok) {
        word = QInputDialog::getText(this, "Znajdź (Enter = Następny)", "Szukaj:", QLineEdit::Normal, word, &ok);

        if (!ok || word.isEmpty()) break;

        // 1. Szukanie (jeśli koniec, leć od początku)
        if (!ui->noteText->find(word)) {
            ui->noteText->moveCursor(QTextCursor::Start);
            ui->noteText->find(word);
        }

        // 2. Licznik (krótka wersja)
        QString content = ui->noteText->toPlainText();
        int total = content.count(word, Qt::CaseInsensitive);
        int current = content.left(ui->noteText->textCursor().selectionEnd()).count(word, Qt::CaseInsensitive);

        // 3. Status
        this->statusBar()->showMessage(QString("Wynik: %1 / %2").arg(current).arg(total));
    }
}

void MainWindow::on_actionZnajdz_i_zamien_triggered()
{
    bool ok1, ok2;
    // 1. Pytamy o słowo do znalezienia
    QString searchWord = QInputDialog::getText(this, "Zamień", "Znajdź:", QLineEdit::Normal, "", &ok1);

    if (ok1 && !searchWord.isEmpty()) {
        // 2. Pytamy o nowe słowo
        QString replaceWord = QInputDialog::getText(this, "Zamień", "Zamień na:", QLineEdit::Normal, "", &ok2);

        if (ok2) {
            // 3. Pobieramy cały tekst z notatnika
            QString content = ui->noteText->toPlainText();

            // 4. Wykonujemy zamianę wszystkich wystąpień
            if (content.contains(searchWord)) {
                content.replace(searchWord, replaceWord);
                ui->noteText->setPlainText(content);
            } else {
                QMessageBox::information(this, "Info", "Nie znaleziono słowa do zamiany.");
            }
        }
    }
}
