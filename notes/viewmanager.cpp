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
// 2. DZIAŁANIE: Tu dzieje się tabulacja (uruchamia się ZA KAŻDYM RAZEM, gdy dotkniesz klawiatury)
bool ViewManager::eventFilter(QObject *obj, QEvent *event) {
    // Reaguj tylko na naciśnięcie klawisza
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // Specjalna obsługa klawisza TAB
        if (keyEvent->key() == Qt::Key_Tab) {
            QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(obj);

            // Jeśli użytkownik zaznaczył tekst, przesuń blok zamiast go usuwać
            if (editor && editor->textCursor().hasSelection()) {
                QTextCursor cursor = editor->textCursor();
                int start = cursor.selectionStart();
                int end = cursor.selectionEnd();

                cursor.beginEditBlock(); // Grupuje zmiany dla funkcji "Cofnij" (Ctrl+Z)

                cursor.setPosition(start);
                cursor.movePosition(QTextCursor::StartOfBlock);

                // Wstawianie spacji na początku każdej linii w zaznaczeniu
                while (cursor.position() < end) {
                    cursor.insertText("    ");
                    if (!cursor.movePosition(QTextCursor::NextBlock)) break;
                    end += 4; // Korekta zakresu po dodaniu znaków
                }

                cursor.endEditBlock();
                return true; // Blokuje standardowe usunięcie tekstu przez Qt
            }
        }
    }
    // Pozostałe klawisze obsługuj normalnie
    return QObject::eventFilter(obj, event);
}

void ViewManager::switchToEdit() { applyStyles(true); }
void ViewManager::switchToPreview() { applyStyles(false); }
