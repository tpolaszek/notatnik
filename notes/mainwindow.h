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
#include <QCompleter>
#include <QStringListModel>
#include "syntaxdictionary.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
\
struct EditorTab {
    QPlainTextEdit *editor = nullptr;
    QPlainTextEdit *lineCounter = nullptr;
    SyntaxHighlighter *highlighter = nullptr;
    QString filePath;
    bool isPreview = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openFileFromPath(const QString &filePath);

private slots:
    void on_openFile_triggered();
    void on_createFile_triggered();
    void on_saveFile_triggered();
    void on_saveFileAs_triggered();

    void on_Find_triggered();

    void on_settings_triggered();
    void on_FindAndReplace_triggered();

    void onTabClose(int index);
    void onTabChange(int index);

    void fillRecentMenu();
    void on_closeFile_triggered();

    bool proceedWithSafetyCheck(int tabIndex);


    void on_about_triggered();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    int currentTabIndex() const;

    EditorTab *currentTab();
    int addTab(const QString &filePath = QString());
    bool closeTab(int index);

    void applyHighlighter(int tabIndex, const QString &filePath);
    void updateTabTitle(int tabIndex);
    void connectEditorSignals(int tabIndex);

    void recordAndRefresh(const QString &filePath);
    void setTitle(QString title);

    Ui::MainWindow *ui;
    QList<EditorTab> tabs;
    QTabWidget *tabWidget;
    QString currentFilePath;
    QString lastSearchTerm;

    SettingsManager *settingsManager;
    SyntaxHighlighter* currentHighlighter;
    GrammarLoader grammarLoader;    
    RecentFilesManager recentMgr;
    ViewManager* viewManager;

    QMenu *recentMenu = nullptr;
    QCompleter *completer = nullptr;
    SyntaxDictionary *syntaxDict = nullptr;;
};

#endif // MAINWINDOW_H
