#ifndef SYNTAXDICTIONARY_H
#define SYNTAXDICTIONARY_H

#include <QObject>
#include <QStringList>
#include <QTextEdit>
#include <QCompleter>
#include <QPlainTextEdit>
#include <QEvent>

class SyntaxDictionary : public QObject
{
    Q_OBJECT
public:
    explicit SyntaxDictionary(QObject *parent = nullptr);

    // Ta funkcja zmienia zestaw słów w zależności od pliku
    void updateLanguageForFile(const QString &filePath, QCompleter *completer);

    // Ta funkcja decyduje, kiedy wyświetlić lub schować dymek z podpowiedziami
    void handleTextChange(QPlainTextEdit *editor, QCompleter *completer);

private:
    // Prywatna funkcja pomocnicza, która zwraca listę słów dla danego rozszerzenia
    QStringList getKeywordsForFile(const QString &filePath);
    // Musimy zapamiętać wskaźniki na czas filtrowania klawiszy
    QPlainTextEdit *currentEditor = nullptr;
    QCompleter *currentCompleter = nullptr;
};

#endif // SYNTAXDICTIONARY_H
