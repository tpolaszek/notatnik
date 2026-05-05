#include "texttools.h"
#include <QScrollBar>
#include <QStatusBar>
#include <QInputDialog>

void TextTools::setupLineCounterUI(QPlainTextEdit *editor, QPlainTextEdit *lineCounter) {
    lineCounter->setReadOnly(true);                // Tylko do odczytu
    lineCounter->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Ukryj suwak

    // Ustawiamy wyrównanie na poziomie DOKUMENTU
    QTextOption option = lineCounter->document()->defaultTextOption();
    option.setAlignment(Qt::AlignCenter);
    lineCounter->document()->setDefaultTextOption(option);

    // Łączymy przewijanie edytora z przewijaniem licznika
    QObject::connect(editor->verticalScrollBar(), &QScrollBar::valueChanged,
                     lineCounter->verticalScrollBar(), &QScrollBar::setValue);
}


void TextTools::updateLineCounter(QPlainTextEdit *editor, QPlainTextEdit *lineCounter) {
    int totalLines = editor->document()->blockCount();
    applyDynamicWidth(totalLines, lineCounter);
    QString lineNumbers;
    for(int i = 1; i<= totalLines; i++) {
        lineNumbers += QString::number(i) + "\n";
    }
    lineCounter->setPlainText(lineNumbers);
    lineCounter->verticalScrollBar()->setValue(editor->verticalScrollBar()->value());
}
void TextTools::applyDynamicWidth(int totalLines, QPlainTextEdit *lineCounter) {
    // 1. Pobieramy informacje o czcionce, która jest aktualnie w liczniku
    QFontMetrics metrics(lineCounter->font());

    // 2. Tworzymy wzorcowy ciąg znaków (najdłuższy możliwy numer linii)
    QString longestLine = QString::number(totalLines);

    // 3. Obliczamy szerokość tego tekstu w pikselach
    // horizontalAdvance mówi nam, ile miejsca zajmie ten string w poziomie
    int textWidth = metrics.horizontalAdvance(longestLine);

    // 4. Dodajemy marginesy (np. 15-20 pikseli), żeby liczby nie dotykały krawędzi
    int finalWidth = textWidth + 25;

    // 5. Ustawiamy szerokość
    lineCounter->setFixedWidth(finalWidth);
}

void TextTools::findText(QMainWindow *parent, QPlainTextEdit *editor, QString &lastSearch) {
    if (lastSearch.isEmpty()) return;

    // Próbujemy znaleźć tekst od aktualnej pozycji kursora
    bool found = editor->find(lastSearch);

    // Jeśli nie znaleziono nic dalej, spróbuj szukać od początku dokumentu (loop)
    if (!found) {
        // Przesuń kursor na początek
        editor->moveCursor(QTextCursor::Start);
        found = editor->find(lastSearch);
    }

    // Jeśli nadal nie znaleziono (słowa w ogóle nie ma w tekście)
    if (!found) {
        // Możesz dodać proste powiadomienie na pasku stanu (StatusBar)
        parent->statusBar()->showMessage("Nie znaleziono: " + lastSearch, 2000);
    }
}
void TextTools::findAndReplace(QMainWindow *parent, QPlainTextEdit *editor) {
    if (!editor || !parent) return;

    // --- 1. INPUT ---
    bool isConfirmed;

    // Pobieramy frazę, którą chcemy zmienić
    QString currentText = QInputDialog::getText(parent, "Znajdź",
                                                "Znajdź tekst", QLineEdit::Normal,
                                                "", &isConfirmed);
    if (!isConfirmed || currentText.isEmpty()) return;

    // Pobieramy frazę docelową
    QString targetText = QInputDialog::getText(parent, "Zamień",
                                               "Zamień na", QLineEdit::Normal,
                                               "", &isConfirmed);
    if (!isConfirmed) return;

    // --- 2. EXECUTION ---
    editor->moveCursor(QTextCursor::Start);
    int replacementCount = 0;

    while (editor->find(currentText)) {
        editor->insertPlainText(targetText);
        replacementCount++;
    }

    // --- 3. STATUS UPDATE ---
    if (replacementCount > 0) {
        // %1 -> " + QString::number(replacementCount) + "
        QString statusMessage = QString("Successfully replaced %1 occurrences.").arg(replacementCount);
        parent->statusBar()->showMessage(statusMessage, 3000);
    } else {
        parent->statusBar()->showMessage("No matches found.", 3000);
    }
}
