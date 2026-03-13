#include "grammarloader.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

void GrammarLoader::loadFromDirectory(const QString& dirPath) {
    QDir dir(dirPath);
    const QStringList jsonFiles = dir.entryList({"*.json"}, QDir::Files);

    for(const QString& fileName : jsonFiles){
        QFile file(dir.filePath(fileName));
        if(!file.open(QFile::ReadOnly)) continue;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if(doc.isNull()) continue;

        QJsonObject grammar = doc.object();
        QString langName = grammar["name"].toString();
        languageMap[langName] = grammar;

        QJsonArray exts = grammar["extensions"].toArray();
        for(const QJsonValue& ext : exts){
            extensionMap[ext.toString()] = grammar;
        }
    }
}

QJsonObject GrammarLoader::grammarForExtension(const QString& ext) const {
    return extensionMap.value(ext, QJsonObject());
}

QStringList GrammarLoader::availableLanguages() const {
    return languageMap.keys();
}
