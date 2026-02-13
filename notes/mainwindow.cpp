#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

class Note{

};

void MainWindow::on_openFile_triggered(){
    QString filter = "Pliki tekstowe (*.txt)";
    QString fileName = QFileDialog::getOpenFileName(this, "Otwórz plik",QDir::homePath(), filter);
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        // Obsługa błędów
        return;
    }

    QTextStream text(&file);
    QString fileContent = text.readAll();
    ui->noteText->setText(fileContent);
}

