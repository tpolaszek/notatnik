#include "texttools.h"
#include <QScrollBar>

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
    int lineCount = editor->document()->blockCount();
    QString lineNumbers;
    for(int i = 1; i<= lineCount; i++) {
        lineNumbers += QString::number(i) + "\n";
    }
    lineCounter->setPlainText(lineNumbers);
}






//TextTools::findText(QMainWindow *parent, QTextEdit *editor, QString &lastSearch) {

//}
