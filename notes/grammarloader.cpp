#include "grammarloader.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

// Metoda wczytuje pliki zawierające instrukcje jak ma podkreślać składnie
void GrammarLoader::loadFromDirectory(const QString& dirPath) {
    QDir dir(dirPath);
    const QStringList jsonFiles = dir.entryList({"*.json"}, QDir::Files);

    //Otwieranie plików i obsługa błędów
    for(const QString& fileName : jsonFiles){
        QFile file(dir.filePath(fileName));
        if(!file.open(QFile::ReadOnly)) continue;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if(doc.isNull()) continue;

        QJsonObject grammar = doc.object();
        QString langName = grammar["name"].toString();
        languageMap[langName] = grammar;

        // Mapa do rozszerzeń plików które będą obsługiwane przez program
        QJsonArray exts = grammar["extensions"].toArray();
        for(const QJsonValue& ext : exts){
            extensionMap[ext.toString()] = grammar;
        }
    }
}

// Zwracanie zasad kolorowania składni do danego rozszerzenia
QJsonObject GrammarLoader::grammarForExtension(const QString& ext) const {
    return extensionMap.value(ext, QJsonObject());
}

// Zwraca dostępne języki
QStringList GrammarLoader::availableLanguages() const {
    return languageMap.keys();
}
