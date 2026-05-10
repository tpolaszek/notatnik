#include "viewmanager.h"
#include "texttools.h"

ViewManager::ViewManager(QPlainTextEdit *editor, QPlainTextEdit *lineCounter, QStatusBar *statusBar, QObject *parent)
    : QObject(parent), m_editor(editor), m_lineCounter(lineCounter), m_statusBar(statusBar) {}

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
// INSTRUKCJA: Tu ustawiamy "wygląd i zasady" (uruchamia się TYLKO RAZ przy starcie)
void ViewManager::setupEditorVisuals(QPlainTextEdit *editor) {
    if (!editor) return;

    // Ustawienie stałej szerokości znaków (kod wygląda równo)
    QFont font("Consolas");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(11);
    editor->setFont(font);

    // Przeliczenie szerokości Tab na dokładnie 4 spacje (w pikselach)
    QFontMetrics metrics(font);
    editor->setTabStopDistance(m_tabSize * metrics.horizontalAdvance(' '));

    // Przekazanie obsługi zdarzeń klawiatury do tej klasy
    editor->installEventFilter(this);

    // Wyłączenie łamania linii (wymusza poziomy pasek przewijania)
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);
}
bool ViewManager::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() != QEvent::KeyPress) return QObject::eventFilter(obj, event);

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(obj);
    if (!editor) return false;

    // 1. Obsługa TAB (Zaznaczenie tekstu)
    if (keyEvent->key() == Qt::Key_Tab && editor->textCursor().hasSelection()) {
        QTextCursor cursor = editor->textCursor();
        int start = cursor.selectionStart(), end = cursor.selectionEnd();
        cursor.beginEditBlock();
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::StartOfBlock);
        while (cursor.position() < end) {
            cursor.insertText("    ");
            if (!cursor.movePosition(QTextCursor::NextBlock)) break;
            end += 4;
        }
        cursor.endEditBlock();
        return true;
    }

    // 2. Obsługa Inteligentnych klawiszy (Enter, Backspace, Nawiasy)
    if (handleSmartKeys(keyEvent, editor)) return true;

    return QObject::eventFilter(obj, event);
}

bool ViewManager::handleSmartKeys(QKeyEvent *keyEvent, QPlainTextEdit *editor) {
    QTextCursor cursor = editor->textCursor();
    int pos = cursor.position();
    int key = keyEvent->key();
    QString opens = "([{", closes = ")]}";

    QChar left = editor->document()->characterAt(pos - 1);
    QChar right = editor->document()->characterAt(pos);
    int idxL = opens.indexOf(left);

    // --- CASE A: BACKSPACE (Usuwanie pary) ---
    if (key == Qt::Key_Backspace && idxL != -1 && right == closes.at(idxL)) {
        cursor.beginEditBlock();
        cursor.deleteChar();
        cursor.deletePreviousChar();
        cursor.endEditBlock();
        return true;
    }

    // --- CASE B: ENTER (Rozbijanie klamer) ---
    if ((key == Qt::Key_Return || key == Qt::Key_Enter) && idxL != -1 && right == closes.at(idxL)) {
        cursor.beginEditBlock();
        cursor.insertText("\n    \n");
        cursor.movePosition(QTextCursor::Left);
        editor->setTextCursor(cursor);
        cursor.endEditBlock();
        return true;
    }

    // --- CASE C: WPISYWANIE (Domykanie) ---
    int idxKey = opens.indexOf(keyEvent->text());
    if (idxKey != -1 && !keyEvent->text().isEmpty()) {
        cursor.insertText(keyEvent->text() + closes.at(idxKey));
        cursor.movePosition(QTextCursor::Left);
        editor->setTextCursor(cursor);
        return true;
    }

    return false;
}

void ViewManager::switchToEdit() { applyStyles(true); }
void ViewManager::switchToPreview() { applyStyles(false); }
