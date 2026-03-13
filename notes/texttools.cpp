#include "texttools.h"
#include <QScrollBar>
#include <QStatusBar>

void TextTools::setupLineCounterUI(QTextEdit *editor, QTextEdit *lineCounter) {
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

void TextTools::updateLineCounter(QTextEdit *editor, QTextEdit *lineCounter) {
    int totalLines = editor->document()->blockCount();
    applyDynamicWidth(totalLines, lineCounter);
    QString lineNumbers;
    for(int i = 1; i<= totalLines; i++) {
        lineNumbers += QString::number(i) + "\n";
    }
    lineCounter->setPlainText(lineNumbers);
}
void TextTools::applyDynamicWidth(int totalLines, QTextEdit *lineCounter) {
    if (totalLines >= 1000) {
        lineCounter->setFixedWidth(45);
    } else {
        lineCounter->setFixedWidth(35);
    }
}

void TextTools::findText(QMainWindow *parent, QTextEdit *editor, QString &lastSearch) {
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






//TextTools::findText(QMainWindow *parent, QTextEdit *editor, QString &lastSearch) {

//}
