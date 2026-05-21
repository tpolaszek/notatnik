#include "syntaxdictionary.h"
#include <QFileInfo>
#include <QStringListModel>
#include <QRegularExpression>
#include <QAbstractItemView>
#include <QKeyEvent>

SyntaxDictionary::SyntaxDictionary(QObject *parent) : QObject(parent)
{
    // Konstruktor może narazie pozostać pusty
}

// 1. Zwracanie słów kluczowych na podstawie rozszerzenia (.cpp, .py, .html)
QStringList SyntaxDictionary::getKeywordsForFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower(); // Wyciąga samo rozszerzenie pliku

    if (ext == "cpp" || ext == "h") {
        return { "int", "float", "char", "double", "class", "struct", "public",
                "private", "protected", "if", "else", "while", "for", "return",
                "include", "switch", "case", "break", "continue", "void", "true", "false" };
    }
    else if (ext == "py") {
        return { "def", "class", "import", "from", "as", "if", "else", "elif",
                "while", "for", "in", "return", "print", "try", "except",
                "lambda", "None", "True", "False", "pass", "break", "continue" };
    }
    else if (ext == "html" || ext == "htm") {
        return { "<html>", "</html>", "<head>", "</head>", "<body>", "</body>",
                "<div>", "</div>", "<p>", "</p>", "<span>", "</span>",
                "<h1>", "</h1>", "<a>", "</a>", "img", "src", "href", "class", "id" };
    }

    // Jeśli plik to zwykły .txt lub nieznany format, nie podpowiadamy nic
    return QStringList();
}

// 2. Ładowanie nowych słów do QCompletera
void SyntaxDictionary::updateLanguageForFile(const QString &filePath, QCompleter *completer)
{
    if (!completer) return;

    // Pobieramy listę słów odpowiadającą plikowi
    QStringList newKeywords = getKeywordsForFile(filePath);

    // Pakujemy ją w model danych akceptowany przez Qt
    QStringListModel *model = new QStringListModel(newKeywords, completer);

    // Wstrzykujemy model do completera (stary model zostanie automatycznie usunięty z pamięci)
    completer->setModel(model);
}

void SyntaxDictionary::handleTextChange(QPlainTextEdit *editor, QCompleter *completer)
{
    if (!editor || !completer) return;

    // ZAPAMIĘTUJEMY WSKAŹNIKI (Dopisujemy te dwie linijki na samym początku starej funkcji)
    currentEditor = editor;
    currentCompleter = completer;

    // Pobieramy cały tekst z edytora i pozycję kursora
    QString text = editor->toPlainText();
    int cursorPosition = editor->textCursor().position();

    if (text.isEmpty() || cursorPosition == 0) {
        completer->popup()->hide();
        return;
    }

    // Wyciągamy podciąg od początku do miejsca, gdzie stoi kursor
    QString textUpToCursor = text.left(cursorPosition);

    // Sprawdzamy ostatni znak PRZED kursosem
    QString lastChar = textUpToCursor.right(1);

    // Aktywujemy tylko dla liter i cyfr
    if (lastChar.contains(QRegularExpression("[a-zA-Z0-9]"))) {

        // Wyciągamy ostatnie słowo pisane przed kursosem
        QString lastWord = textUpToCursor.split(QRegularExpression("\\s+")).last();

        // Czyścimy i ustawiamy prefix (to filtruje listę podpowiedzi)
        completer->setCompletionPrefix(lastWord);

        // Obliczamy pozycję dymka wewnątrz edytora tekstowego
        QRect cr = editor->cursorRect();

        // Przesuwamy okienko lekko w dół, żeby nie zasłaniało wpisywanej litery
        cr.setWidth(completer->popup()->sizeHint().width());

        // Wymuszamy zaznaczenie pierwszego pasującego elementu na liście
        completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));

        // Najważniejsze: wywołujemy pokazanie dymka na ekranie
        completer->complete(cr);
    }
    else {
        // Jeśli użytkownik kliknął spację, enter itp. - chowamy podpowiedzi
        completer->popup()->hide();
    }
}
