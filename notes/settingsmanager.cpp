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
    QDir dir(":/themes");
    QStringList result;
    for (const QString &file : dir.entryList({"*.css"}, QDir::Files))
        result << dir.filePath(file);
    return result;
}

// Zwraca aktualną ścieżke pliku motywu
QString SettingsManager::currentThemePath() const {
    return themePath;
}

// Ustawia motyw
void SettingsManager::setThemePath(const QString &path) {
    themePath = path;
}

// Zwraca domyślną wielkość czcionki
int SettingsManager::fontSize() const {
    return defaultfontSize;
}

// Ustawia rozmiar czcionki
void SettingsManager::setFontSize(int size) {
    defaultfontSize = size;
}

// Zapisuje ustawienia
void SettingsManager::save() {
    QSettings s("Ustawienie", "Notatnik");
    s.setValue("theme/path", themePath);
    s.setValue("editor/fontSize", defaultfontSize);
}

// Wczytuje zapisane ustawienie
void SettingsManager::load() {
    QSettings s("Ustawienie", "Notatnik");
    themePath = s.value("theme/path", QString()).toString();
    defaultfontSize = s.value("editor/fontSize", 12).toInt();
}

// Zamienia css na utf-8
QString SettingsManager::composeStylesheet() const {
    QString css;

    if (!themePath.isEmpty()) {
        QFile f(themePath);
        if (f.open(QFile::ReadOnly | QFile::Text))
            css = QString::fromUtf8(f.readAll());
    }

    return css;
}

// Aplikuje zmieniony w metodzie composeStyleSheet css do aplikacji
void SettingsManager::applyToApp() {
    qApp->setStyleSheet(composeStylesheet());
}
