#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Ustawienie domyślnej nazwy okienka jako notatnik przy tworzeniu obiektu
    this->setWindowTitle("Notatnik");
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

