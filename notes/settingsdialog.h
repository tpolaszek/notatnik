#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QSpinBox>
#include <QLabel>
#include "settingsmanager.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(SettingsManager *manager, QWidget *parent = nullptr);

private slots:
    void onThemeSelected(QListWidgetItem *item);
    void onBrowseClicked();
    void onApplyClicked();
    void onOkClicked();

private:
    void buildUI();
    void populateThemeList();
    void selectCurrentTheme();

    SettingsManager *manager;

    QListWidget *themeList;
    QLabel      *previewLabel;
    QSpinBox    *fontSizeBox;
};

#endif // SETTINGSDIALOG_H