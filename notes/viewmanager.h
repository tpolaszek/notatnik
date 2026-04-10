#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QPlainTextEdit>
#include <QStatusBar>

class ViewManager {
public:
    ViewManager(QPlainTextEdit *editor, QPlainTextEdit *lineCounter, QStatusBar *statusBar);

    void switchToEdit();    // Tryb edycji
    void switchToPreview(); // Tryb podglądu (tylko do odczytu)

private:
    QPlainTextEdit *m_editor;
    QPlainTextEdit *m_lineCounter;
    QStatusBar *m_statusBar;

    void applyStyles(bool isEditing);
};

#endif
