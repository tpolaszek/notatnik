#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QPlainTextEdit>
#include <QStatusBar>
#include <QObject>
#include <QCompleter>

class ViewManager : public QObject {
    Q_OBJECT

public:
    ViewManager(QPlainTextEdit *editor, QPlainTextEdit *lineCounter, QStatusBar *statusBar, QObject *parent = nullptr);

    void applyFontSize(int pointSize);

    void switchToEdit();
    void switchToPreview();
    void setupEditorVisuals(QPlainTextEdit *editor);
    void setCompleter(QCompleter *c) { m_completer = c; } // Szybki setter

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QPlainTextEdit *m_editor;
    QPlainTextEdit *m_lineCounter;
    QStatusBar *m_statusBar;
    QCompleter *m_completer = nullptr;
    const int m_tabSize = 4;

    bool handleSmartKeys(QKeyEvent *keyEvent, QPlainTextEdit *editor);
    void applyStyles(bool isEditing);

};

#endif
