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
