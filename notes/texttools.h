#ifndef TEXTTOOLS_H
#define TEXTTOOLS_H

#include <QString>
#include <QTextEdit>
#include <QMainWindow>

class TextTools
{
public:
    static void updateLineCounter(QTextEdit *editor, QTextEdit *lineCounter);
    static void setupLineCounterUI(QTextEdit *editor, QTextEdit *lineCounter);
    static void findText(QMainWindow *parent, QTextEdit *editor, QString &lastSearch);
    static void findAndReplace(QMainWindow *parent, QTextEdit *editor);
};

#endif // TEXTTOOLS_H
