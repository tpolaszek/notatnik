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
    return QFileDialog::getOpenFileName(parent, "Otwórz plik", QDir::homePath(),
    "Wszystkie obsługiwane (*.txt *.html *.cpp *.h *.py);;"
    "Pliki tekstowe (*.txt);;"
    "HTML (*.html);;"
    "C++ (*.cpp *.h);;"
    "Python (*.py *.pyw)");
}

// Metoda zapisuje plik do przekazanej ścieżki
bool FileHandling::saveFile(const QString &filePath, const QString &content){
    QFile file(filePath);
    if(!file.open(QFile::WriteOnly | QFile::Text))return false;
    QTextStream out(&file);
    out << content;
    file.close();

    return true;
}

// Pobiera ścieżke pliku
QString FileHandling::getSaveFilePath(QWidget *parent){
    return QFileDialog::getSaveFileName(parent, "Zapisz jako", "",
    "Wszystkie obsługiwane (*.txt *.html *.cpp *.h *.py);;"
    "Pliki tekstowe (*.txt);;"
    "HTML (*.html);;"
    "C++ (*.cpp *.h);;"
    "Python (*.py *.pyw)");
}
