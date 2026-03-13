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

private:
    QMap<QString, QJsonObject> extensionMap;
    QMap<QString, QJsonObject> languageMap;
};

#endif // GRAMMARLOADER_H
