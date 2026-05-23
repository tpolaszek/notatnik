#include "syntaxdictionary.h"
#include <QFileInfo>
#include <QStringListModel>
#include <QRegularExpression>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SyntaxDictionary::SyntaxDictionary(QObject *parent) : QObject(parent)
{
    // Konstruktor może narazie pozostać pusty
}

QStringList SyntaxDictionary::getKeywordsForFile(const QString &filePath)
{
    // Pobiera rozszerzenie pliku i zamienia na małe litery (np. "cpp", "py")
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();

    // Tworzy dynamiczną ścieżkę do pliku JSON w zasobach Qt
    QString jsonPath = ":/dictionary/dictionary/" + ext + ".json";
    QFile file(jsonPath);

    // ZABEZPIECZENIE: Jeśli nie ma takiego JSON-a, kończy bezpiecznie bez crasha
    if (!file.open(QFile::ReadOnly)) {
        return QStringList();
    }

    // Wczytuje surowe dane tekstowe i zamyka plik
    QByteArray fileData = file.readAll();
    file.close();

    // ZABEZPIECZENIE: Przetwarza tekst na JSON i sprawdza, czy plik nie jest uszkodzony
    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isObject()) {
        return QStringList();
    }

    // Wyciąga główny obiekt JSON oraz tworzy listę na słowa
    QJsonObject jsonObject = doc.object();
    QStringList words;

    // Sprawdza, czy istnieje lista o nazwie "keywords" i przepisuje ją do pamięci programu
    if (jsonObject.contains("keywords") && jsonObject["keywords"].isArray()) {
        QJsonArray jsonArray = jsonObject["keywords"].toArray();
        for (int i = 0; i < jsonArray.size(); ++i) {
            words.append(jsonArray.at(i).toString());
        }
    }

    // Zwraca gotową listę słów kluczowych do kompletera
    return words;
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
