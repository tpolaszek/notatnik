#ifndef GRAMMARLOADER_H
#define GRAMMARLOADER_H


#include <QString>
#include <QMap>
#include <QJsonObject>

class GrammarLoader {
public:
    void loadFromDirectory(const QString& dirPath);
    QJsonObject grammarForExtension(const QString& ext) const;
    QStringList availableLanguages() const;
    static QJsonArray getMergedArray(const QJsonObject& baseJson, const QString& folderPath, const QString& keyName);


private:
    QMap<QString, QJsonObject> extensionMap;
    QMap<QString, QJsonObject> languageMap;
};

#endif // GRAMMARLOADER_H
