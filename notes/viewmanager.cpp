#include "viewmanager.h"
#include "texttools.h"
#include <QApplication>
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QLayout>

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

void ViewManager::applyFontSize(int pointSize){
    QFont font("Consolas");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(pointSize);

    m_editor->setFont(font); // ustawia wielkość czcionki do edytora
    m_lineCounter->setFont(font); // ustawia wielkość czcionki do licznika liń

    QFontMetrics metrics(font);
    m_editor->setTabStopDistance(m_tabSize * metrics.horizontalAdvance(' ')); // ustawia odległość tabulatora

    int totalLines = m_editor->document()->blockCount();
    TextTools::applyDynamicWidth(totalLines, m_lineCounter, &font);

    m_lineCounter->updateGeometry();
    m_lineCounter->parentWidget()->layout()->activate();
}

// INSTRUKCJA: Tu ustawiamy "wygląd i zasady" (uruchamia się TYLKO RAZ przy starcie)
void ViewManager::setupEditorVisuals(QPlainTextEdit *editor) {
    if (!editor) return;

    // Ustawienie stałej szerokości znaków (kod wygląda równo)
    QFont font("Consolas");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(11);
    editor->setFont(font);
    m_lineCounter->setFont(font);

    // Przeliczenie szerokości Tab na dokładnie 4 spacje (w pikselach)
    QFontMetrics metrics(editor->font());
    editor->setTabStopDistance(m_tabSize * metrics.horizontalAdvance(' '));

    // Przekazanie obsługi zdarzeń klawiatury do tej klasy
    editor->installEventFilter(this);

    // Wyłączenie łamania linii (wymusza poziomy pasek przewijania)
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    connect(editor, &QPlainTextEdit::cursorPositionChanged, editor, [editor]() {
        QWidget line; // tworzenie obiektu linia
        line.setObjectName("currentLineColor"); // ustawienie nazwy obiektu (głównie do cssa potrzebowałem)
        line.setStyleSheet(qApp->styleSheet()); // stosuje styl w plikach
        line.ensurePolished();
        QColor color = line.palette().color(QPalette::Window);

        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(color); // ustawia tło linii
        selection.format.setProperty(QTextFormat::FullWidthSelection, true); // ustawia formatowanie na całą linie
        selection.cursor = editor->textCursor();  // ustanawia gdzie jest kursor tekstu
        selection.cursor.clearSelection(); // usuwa poprzednie zaznaczenie przez ustawienie "kotwicy" na aktualnej pozycji kursora
        editor->setExtraSelections({selection});
    });
}

bool ViewManager::eventFilter(QObject *obj, QEvent *event) {
    if(!obj || !event) return false;
    if (event->type() != QEvent::KeyPress) return QObject::eventFilter(obj, event);
    if (event->type() == QEvent::StyleChange || event->type() == QEvent::PaletteChange) {
        return QObject::eventFilter(obj, event);
    }

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // ====================================================================
    // KROK 0: OBSŁUGA SŁOWNIKA (Z inteligentną przepustowością)
    // ====================================================================
    if (m_completer && m_completer->popup()) {

        // WARUNEK KLUCZOWY: Reagujemy tylko, jeśli dymek jest FIZYCZNIE WIDOCZNY
        // oraz kompleter ma dla nas jakieś pasujące słowo!
        bool isPopupActive = m_completer->popup()->isVisible() && !m_completer->currentCompletion().isEmpty();

        if (isPopupActive) {
            if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {

                QModelIndex idx = m_completer->popup()->currentIndex();
                QString currentCompletion = m_completer->completionModel()->data(idx).toString();
                if (currentCompletion.isEmpty()) currentCompletion = m_completer->currentCompletion();
                emit m_completer->activated(currentCompletion);

                m_completer->popup()->hide();
                return true; // Blokujemy spacje/entery TYLKO gdy dymek podpowiadał
            }

            if (keyEvent->key() == Qt::Key_Escape) {
                m_completer->popup()->hide();
                return true;
            }
        }
    }

    // Rzutowanie na edytor dla standardowych zachowań
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(obj);
    if (!editor) return QObject::eventFilter(obj, event);

    // ====================================================================
    // 1. STANDARDOWA OBSŁUGA TAB (Działa zawsze, gdy dymek jest zamknięty/pusty)
    // ====================================================================
    if (keyEvent->key() == Qt::Key_Tab) {
        // PRZYPADEK A: Tekst jest zaznaczony -> wcinamy cały blok
        if (editor->textCursor().hasSelection()) {
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
        // PRZYPADEK B: Brak zaznaczenia i brak dymka -> wstawiamy standardowe 4 spacje
        else {
            editor->insertPlainText("    ");
            return true;
        }
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
// Kontroluje zawsze były 3 linie wyświetlane na końcu
void ViewManager::setupBottomSpace(QPlainTextEdit *editor) {
    // 1. Bezpieczeństwo: sprawdzamy, czy edytor w ogóle istnieje
    if (!editor) return;

    // 2. Podpinamy się pod sygnał zmiany pozycji kursora
    QObject::connect(editor, &QPlainTextEdit::cursorPositionChanged, editor, [editor]() {

        // 3. Pobieramy pionowy suwak (scrollbary) edytora
        QScrollBar *vBar = editor->verticalScrollBar();
        if (!vBar) return;

        // 4. Pobieramy numer bieżącej linii kursora oraz łączną liczbę linii
        int currentLine = editor->textCursor().blockNumber();
        int totalLines = editor->document()->blockCount();

        // 5. Jeśli kursor jest na jednej z 3 ostatnich linii (strefa przykońcowa)...
        if (totalLines - currentLine <= 3) {
            // ...bezwzględnie dopychamy suwak do samego dołu,
            // dzięki czemu nawias } zawsze wyskoczy na ekran!
            vBar->setValue(vBar->maximum());
        }
    });
}

void ViewManager::switchToEdit() { applyStyles(true); }

void ViewManager::switchToPreview() { applyStyles(false); }
