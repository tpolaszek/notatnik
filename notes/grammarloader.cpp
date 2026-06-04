#include "grammarloader.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

// Metoda wczytuje pliki zawierające instrukcje jak ma podkreślać składnie
void GrammarLoader::loadFromDirectory(const QString& dirPath) {
    QDir dir(dirPath);
    const QStringList jsonFiles = dir.entryList({"*.json"}, QDir::Files);

    // Otwieranie plików i obsługa błędów
    for(const QString& fileName : jsonFiles){
        QString fullPath = dirPath + "/" + fileName;
        QFile file(fullPath);

        if(!file.open(QFile::ReadOnly)){
            qDebug() << "Failed to open:" << file.errorString();
            continue;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if(doc.isNull()) continue;

        QJsonObject grammar = doc.object();
        QString langName = grammar["name"].toString();
        languageMap[langName] = grammar;

        // Mapa do rozszerzeń plików które będą obsługiwane przez program
        QJsonArray exts = grammar["extensions"].toArray();

        // Dla każdego rozszerzenia dodaje do mapy dodaje zasady podkreślania sładni
        for(const QJsonValue& ext : exts){
            extensionMap[ext.toString()] = grammar;
        }
    }
}

// ====================================================================
// NOWA FUNKCJA POMOCNICZA (STATYCZNA): Odpowiada za scalanie JSON-ów
// ====================================================================
QJsonArray GrammarLoader::getMergedArray(const QJsonObject& baseJson, const QString& folderPath, const QString& keyName) {
    QJsonArray combinedArray;

    // 1. Sprawdzamy, czy plik bazowy żąda dołączenia innych plików przez "includes"
    if (baseJson.contains("includes") && baseJson["includes"].isArray()) {
        QJsonArray includes = baseJson["includes"].toArray();

        // PĘTLA 1: Przechodzimy po kolei przez każdy dopisany plik (np. js.json, css.json)
        for (const QJsonValue& incVal : includes) {
            QString incFileName = incVal.toString(); // np. "js.json"
            QFile file(folderPath + incFileName);     // np. ":/syntax/syntax/js.json"

            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QJsonObject subJson = QJsonDocument::fromJson(file.readAll()).object();

                // Jeśli wywołuje się słownik to mamy keywords, a jeśli kolorki to mamy rules
                if (subJson.contains(keyName) && subJson[keyName].isArray()) {
                    QJsonArray subArray = subJson[keyName].toArray();
                    // PĘTLA 2: Przepisujemy każdy pojedynczy element z pod-pliku do naszego wspólnego worka
                    for (const QJsonValue& item : subArray) {
                        combinedArray.append(item);
                    }
                }
                file.close();
            }
        }
    }

    // 2. Na koniec dorzucamy elementy z pliku głównego (np. html.json), żeby były na dole
    if (baseJson.contains(keyName) && baseJson[keyName].isArray()) {
        QJsonArray baseArray = baseJson[keyName].toArray();
        // PĘTLA 3: Przepisujemy elementy z pliku głównego na sam koniec naszej listy
        for (const QJsonValue& item : baseArray) {
            combinedArray.append(item);
        }
    }

    return combinedArray;
}

// Zwracanie zasad kolorowania składni do danego rozszerzenia
QJsonObject GrammarLoader::grammarForExtension(const QString& ext) const {
    return extensionMap.value(ext, QJsonObject());
}

// Zwraca dostępne języki
QStringList GrammarLoader::availableLanguages() const {
    return languageMap.keys();
}
