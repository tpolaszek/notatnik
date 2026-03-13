#ifndef TEXTTOOLS_H
#define TEXTTOOLS_H

#include <QString>
#include <QTextEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
class TextTools
{
public:
    static void updateLineCounter(QPlainTextEdit *editor, QPlainTextEdit *lineCounter);
    static void setupLineCounterUI(QPlainTextEdit *editor, QPlainTextEdit *lineCounter);
    static void findText(QMainWindow *parent, QPlainTextEdit *editor, QString &lastSearch);
    static void findAndReplace(QMainWindow *parent, QPlainTextEdit *editor);
};

#endif // TEXTTOOLS_H
