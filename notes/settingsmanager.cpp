#include "settingsmanager.h"
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QApplication>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
    load();
}

// Skanuje folder /themes w folderze aplikacji
QStringList SettingsManager::availableThemes() const {
    QDir dir(QApplication::applicationDirPath() + "/themes");
    QStringList result;
    for (const QString &file : dir.entryList({"*.css"}, QDir::Files))
        result << dir.filePath(file);
    return result;
}

QString SettingsManager::currentThemePath() const {
    return themePath;
}

void SettingsManager::setThemePath(const QString &path) {
    themePath = path;
}

int SettingsManager::fontSize() const {
    return defaultfontSize;
}

void SettingsManager::setFontSize(int size) {
    defaultfontSize = size;
}

void SettingsManager::save() {
    QSettings s("MyCompany", "Notatnik");
    s.setValue("theme/path", themePath);
    s.setValue("editor/fontSize", defaultfontSize);
}

void SettingsManager::load() {
    QSettings s("MyCompany", "Notatnik");
    themePath = s.value("theme/path", QString()).toString();
    defaultfontSize  = s.value("editor/fontSize", 12).toInt();
}

// Aplikuje zmiany do pliku css
QString SettingsManager::composeStylesheet() const {
    QString css;

    if (!themePath.isEmpty()) {
        QFile f(themePath);
        if (f.open(QFile::ReadOnly | QFile::Text))
            css = QString::fromUtf8(f.readAll());
    }

    // Nadpisz zmiane wielkości czcionki do pliku
    css += QString("\nQPlainTextEdit { font-size: %1pt; }").arg(defaultfontSize);

    return css;
}

void SettingsManager::applyToApp() {
    qApp->setStyleSheet(composeStylesheet());
}