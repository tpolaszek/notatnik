#ifndef TEXTTOOLS_H
#define TEXTTOOLS_H

#include <QString>
#include <QTextEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
class TextTools
{
public:
<<<<<<< HEAD
    static void updateLineCounter(QPlainTextEdit *editor, QPlainTextEdit *lineCounter);
    static void setupLineCounterUI(QPlainTextEdit *editor, QPlainTextEdit *lineCounter);
    static void findText(QMainWindow *parent, QPlainTextEdit *editor, QString &lastSearch);
    static void findAndReplace(QMainWindow *parent, QPlainTextEdit *editor);
=======
    static void updateLineCounter(QTextEdit *editor, QTextEdit *lineCounter);
    static void applyDynamicWidth(int lineCount, QTextEdit *lineCounter);
    static void setupLineCounterUI(QTextEdit *editor, QTextEdit *lineCounter);
    static void findText(QMainWindow *parent, QTextEdit *editor, QString &lastSearch);
    static void findAndReplace(QMainWindow *parent, QTextEdit *editor);
>>>>>>> tools
};

#endif // TEXTTOOLS_H
