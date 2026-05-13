#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QWidget>
#include "recentfilesmanager.h"
#include "mainwindow.h"
#include <QListWidgetItem>

namespace Ui {
class StartScreen;
}

class StartScreen : public QWidget
{
    Q_OBJECT

public:
    explicit StartScreen(MainWindow *mainWindow, QWidget *parent = nullptr);
    ~StartScreen();


private slots:
    void onCreateFileClicked();
    void onOpenFileClicked();
    void onRecentFileDoubleClicked(QListWidgetItem *item);

private:
    void populateRecentFilesList();
    QWidget *titleBar = nullptr;

    MainWindow *mainWindow;
    Ui::StartScreen *ui;
    RecentFilesManager recentMgr;
    QPoint dragPos;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // STARTSCREEN_H
