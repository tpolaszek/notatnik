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
    QString filePath = FileHandling::getOpenFilePath(this);

    // Obsługa błędów
    if(filePath.isEmpty()) return;

    // Wyświetlanie zawartości
    QString content = FileHandling::openFile(filePath);
    if(!content.isNull()){
        ui->noteText->setText(content);
        currentFilePath = filePath;
    }
}
