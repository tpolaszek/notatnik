#include "viewmanager.h"
#include "texttools.h" // Zakładam, że tu masz updateLineCounter

ViewManager::ViewManager(QPlainTextEdit *editor, QPlainTextEdit *lineCounter, QStatusBar *statusBar)
    : m_editor(editor), m_lineCounter(lineCounter), m_statusBar(statusBar) {}

void ViewManager::applyStyles(bool isEditing) {
    m_editor->setReadOnly(!isEditing);

    // Ustawienie flag interakcji
    m_editor->setTextInteractionFlags(isEditing ? Qt::TextEditorInteraction: (Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    // Zarządzanie licznikiem linii
    m_lineCounter->setVisible(isEditing);

    if (isEditing) {
        m_statusBar->showMessage("Tryb: edycji");
        TextTools::updateLineCounter(m_editor, m_lineCounter);
    } else {
        m_statusBar->showMessage("Tryb: przeglądania");
    }
}

void ViewManager::switchToEdit() { applyStyles(true); }
void ViewManager::switchToPreview() { applyStyles(false); }
