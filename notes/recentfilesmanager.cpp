#include "recentfilesmanager.h"

#include <QSettings>
#include <QFileInfo>

RecentFilesManager::RecentFilesManager(QObject *parent) : QObject(parent) {
    load();
}

void RecentFilesManager::recordFile(const QString &filePath) {
    if(filePath.isEmpty()) return;

    for(int i = 0; i<files.size(); ++i){
        if(files[i].path == filePath){
            files.removeAt(i);
            break;
        }
    }

    RecentFile rf;
    rf.path = filePath;
    QFileInfo info(filePath);

    if(info.exists()){
        rf.lastModified = info.lastModified();
    } else {
        rf.lastModified = QDateTime::currentDateTime();
    }

    files.prepend(rf);

    while(files.size() > MaxEntries){
        files.removeLast();
    }

    save();
}

QList<RecentFile> RecentFilesManager::recentFiles() const{
    return files;
}

void RecentFilesManager::load(){
    QSettings s("nazwa", "Notatnik");
    int count = s.beginReadArray("recentFiles");
    files.clear();

    for(int i = 0; i<count; ++i){
        s.setArrayIndex(i);
        RecentFile rf;
        rf.path = s.value("path").toString();
        rf.lastModified = s.value("lastModified").toDateTime();
        if(!rf.path.isEmpty()) files.append(rf);
    }
    s.endArray();
}

void RecentFilesManager::save() const{
    QSettings s("nazwa","Notatnik");
    s.beginWriteArray("recentFiles", files.size());

    for(int i = 0; i< files.size(); ++i){
        s.setArrayIndex(i);
        s.setValue("path", files[i].path);
        s.setValue("lastModified", files[i].lastModified);
    }
    s.endArray();
}
