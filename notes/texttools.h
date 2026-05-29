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
    static void applyDynamicWidth(int lineCount, QPlainTextEdit *lineCounter, const QFont *font = nullptr);
    static int getCount(QPlainTextEdit *editor, const QString &searchTerm);

};

#endif // TEXTTOOLS_H
