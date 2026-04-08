#include "filehandling.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include <QFileDialog>
// Metoda otwiera plik
QString FileHandling::openFile(const QString &filePath) {
    QFile file(filePath);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        // Obsługa błędów
        return QString();
    }

    QTextStream in(&file);
    return in.readAll();
}

// Metoda pobiera ścieżkę otwartego pliku
QString FileHandling::getOpenFilePath(QWidget *parent){
    return QFileDialog::getOpenFileName(parent, "Otwórz plik",QDir::homePath(), "Pliki tekstowe (*.txt);; HTML (*.html)"/* miejsce na rozbudowe wspieranie inntcg plików*/);
}

bool FileHandling::saveFile(const QString &filePath, const QString &content){
    QFile file(filePath);
    if(file.open(QFile::WriteOnly | QFile::Text))return false;
    QTextStream out(&file);
    out << content;
    file.close();

    return true;
}

QString FileHandling::getSaveFilePath(QWidget *parent){
    return QFileDialog::getSaveFileName(parent, "Zapisz jako", "", "Pliki tekstowe (*.txt);;HTML (*.html);;Wszystkie pliki(*)");
}
