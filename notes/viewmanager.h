#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QPlainTextEdit>
#include <QStatusBar>
#include <QObject>

class ViewManager : public QObject {
    Q_OBJECT

public:
    ViewManager(QPlainTextEdit *editor, QPlainTextEdit *lineCounter, QStatusBar *statusBar, QObject *parent = nullptr);

    void switchToEdit();
    void switchToPreview();
    void setupEditorVisuals(QPlainTextEdit *editor);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QPlainTextEdit *m_editor;
    QPlainTextEdit *m_lineCounter;
    QStatusBar *m_statusBar;
    const int m_tabSize = 4;

    bool handleSmartKeys(QKeyEvent *keyEvent, QPlainTextEdit *editor);
    void applyStyles(bool isEditing);
};

#endif
