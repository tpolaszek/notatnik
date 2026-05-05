#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filehandling.h"
#include "grammarloader.h"
#include "syntaxhighlighter.h"
#include "settingsmanager.h"
#include "settingsdialog.h"
#include "recentfilesmanager.h"
#include "viewmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setTitle(QString title);

    void openFileFromPath(const QString &filePath);

private slots:
    void on_openFile_triggered();
    void on_createFile_triggered();
    void on_saveFile_triggered();
    void on_saveFileAs_triggered();

    void on_Find_triggered();

    void on_settings_triggered();
    void on_FindAndReplace_triggered();

    void fillRecentMenu();
    void on_closeFile_triggered();

private:
    void applyHighlighter(const QString &filePath);
    void recordAndRefresh(const QString &filePath);

    Ui::MainWindow* ui;
    QString currentFilePath;
    QString lastSearchTerm;

    SettingsManager *settingsManager;
    SyntaxHighlighter* currentHighlighter;
    GrammarLoader grammarLoader;    
    RecentFilesManager recentMgr;
    ViewManager* viewManager;

    QMenu *recentMenu = nullptr;
};

#endif // MAINWINDOW_H
