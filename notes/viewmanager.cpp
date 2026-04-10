#include "viewmanager.h"
#include "texttools.h" // Zakładam, że tu masz updateLineCounter

ViewManager::ViewManager(QPlainTextEdit *editor, QPlainTextEdit *lineCounter, QStatusBar *statusBar)
    : m_editor(editor), m_lineCounter(lineCounter), m_statusBar(statusBar) {}

void ViewManager::applyStyles(bool isEditing) {
    m_editor->setReadOnly(!isEditing);

    // Ustawienie flag interakcji
    m_editor->setTextInteractionFlags(isEditing ? Qt::TextEditorInteraction
                                                : (Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));

    // Zarządzanie licznikiem linii
    m_lineCounter->setVisible(isEditing);

    // Wygląd
    if (isEditing) {
        m_editor->setStyleSheet(""); // Przywróć domyślny
        m_statusBar->showMessage("Mode: Editing");
        TextTools::updateLineCounter(m_editor, m_lineCounter);
        m_editor->setFocus();
    } else {
        m_editor->setStyleSheet("background-color: #f4f4f4; color: #555555;");
        m_statusBar->showMessage("Mode: Preview (Read-Only)");
    }
}

void ViewManager::switchToEdit() { applyStyles(true); }
void ViewManager::switchToPreview() { applyStyles(false); }
