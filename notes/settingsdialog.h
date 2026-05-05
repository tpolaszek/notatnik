#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class SettingsWindow;
}

class SettingsManager;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(SettingsManager *manager, QWidget *parent = nullptr);
    ~SettingsDialog();

private:
    Ui::SettingsWindow *ui;
    SettingsManager *manager;

    void populateThemeList();
    void selectCurrentTheme();

private slots:
    void onThemeSelected(QListWidgetItem *item);
    void onBrowseClicked();
    void onApplyClicked();
    void onOkClicked();
};

#endif
