#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

class SettingsManager : public QObject {
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);

    QStringList availableThemes() const;
    QString currentThemePath() const;
    void setDefaultTheme();
    void setThemePath(const QString &path);

    int fontSize() const;
    void setFontSize(int size);

    void save();
    void load();
    void applyToApp();

private:
    QString composeStylesheet() const;

    QString themePath;
    int defaultfontSize = 12;
};

#endif // SETTINGSMANAGER_H
