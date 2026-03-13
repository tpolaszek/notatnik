#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "texttools.h"
#include <QFileDialog>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Notatnik");

    // Ustawiam wygląd licznika
    TextTools::setupLineCounterUI(ui->noteText, ui->lineCounter);

    // Za każdym razem, gdy zmienisz tekst w noteText, wywoła się updateLineCounter
    connect(ui->noteText, &QTextEdit::textChanged, this, [this]() {
        TextTools::updateLineCounter(ui->noteText, ui->lineCounter);
    });
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
        ui->noteText->setText(content);
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
