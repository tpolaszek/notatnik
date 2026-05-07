#include "recentfilesmanager.h"

#include <QSettings>
#include <QFileInfo>

RecentFilesManager::RecentFilesManager(QObject *parent) : QObject(parent) {
    load();
}

// zapisuje otwarte poprzednio pliki
void RecentFilesManager::recordFile(const QString &filePath) {
    if(filePath.isEmpty()) return; // jeśli nie ma tego pliku nic nie zwraca

    for(int i = 0; i<files.size(); ++i){
        if(files[i].path == filePath){
            files.removeAt(i);
            break;
        }
    }

    RecentFile rf;
    rf.path = filePath;
    QFileInfo info(filePath); // informacje o pliku

    // jeśli istnieją informacje o pliku
    if(info.exists()){
        rf.lastModified = info.lastModified(); // ostatni plik ma informacje sprawdzanego pliku
    } else {
        rf.lastModified = QDateTime::currentDateTime(); // jeśli nie ma informacji to dodaje aktualną date i czas
    }

    files.prepend(rf); // dodaje na początku listy ostatni plik

    // jeśli ilość plików na liście jest większa od maksymalnej ilości
    while(files.size() > MaxEntries){
        files.removeLast(); // usuwa ostatnie pliki z listy
    }

    save();
}

// Metoda zwraca liste ostatnich plików
QList<RecentFile> RecentFilesManager::recentFiles() const{
    return files;
}

// Ładuje ostatnie pliki
void RecentFilesManager::load(){
    QSettings s("UstawienieOstatniePliki", "Notatnik");
    int count = s.beginReadArray("recentFiles");
    files.clear(); // czyści liste plików

    // Póki i jest mniejsze od ilości plików
    for(int i = 0; i<count; ++i){
        s.setArrayIndex(i);
        RecentFile rf;
        rf.path = s.value("path").toString();
        rf.lastModified = s.value("lastModified").toDateTime();
        if(!rf.path.isEmpty()) files.append(rf);
    }
    s.endArray(); // zamknij tablice
}

// Zapisuje ostatnie pliki
void RecentFilesManager::save() const{
    QSettings s("UstawienieOstatniePliki","Notatnik");
    s.beginWriteArray("recentFiles", files.size());

    for(int i = 0; i< files.size(); ++i){
        s.setArrayIndex(i);
        s.setValue("path", files[i].path);
        s.setValue("lastModified", files[i].lastModified);
    }
    s.endArray();
}
