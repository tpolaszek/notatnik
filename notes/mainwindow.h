#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filehandling.h"
#include "grammarloader.h"
#include "syntaxhighlighter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setTitle(QString title);

private slots:
    void on_openFile_triggered();
    void on_createFile_triggered();
    void on_saveFile_triggered();
    void on_saveFileAs_triggered();

    void on_actionFind_triggered();

private:
    void applyHighlighter(const QString& filePath);

    Ui::MainWindow* ui;
    QString currentFilePath;

    SyntaxHighlighter* currentHighlighter;
    GrammarLoader grammarLoader;
    QString lastSearchTerm;

};

#endif // MAINWINDOW_H