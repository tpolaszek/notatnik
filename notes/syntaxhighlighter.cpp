#include "syntaxhighlighter.h"
#include "grammarloader.h"
#include <QJsonArray>
#include <QFont>
#include <QFile>
#include <QJsonDocument>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {}

// Główna metoda ładowania zasad z pliku JSON -> (Przygotowanie zasad)
void SyntaxHighlighter::loadFromJson(const QJsonObject& grammar){
    rules.clear(); // Resetujemy stare reguły kolorowania

    // Jedna linijka załatwia cały proces "includes" oraz wczytanie reguł bazowych!
    QJsonArray allRules = GrammarLoader::getMergedArray(grammar, ":/syntax/syntax/", "rules");

    // Teraz tylko przepisujemy gotową, scaloną tablicę do naszej listy reguł
    for (const QJsonValue& val : allRules){
        QJsonObject ruleObj = val.toObject();
        HighlightRule rule;

        rule.pattern = QRegularExpression(ruleObj["pattern"].toString());

        QTextCharFormat fmt;
        fmt.setForeground(QColor(ruleObj["color"].toString()));
        if(ruleObj["bold"].toBool()) fmt.setFontWeight(QFont::Bold);
        if(ruleObj["italic"].toBool()) fmt.setFontItalic(true);

        rule.format = fmt;

        // ZAPIS: Wrzucamy przygotowaną regułę do wspólnego kontenera klasy, by zapisać ją w pamięci
        rules.append(rule);
    }
}

// Metoda podkreśla składnię (Wykorzystanie zasad w praktyce)
void SyntaxHighlighter::highlightBlock(const QString& text){
    for(const HighlightRule & rule : rules){
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while(it.hasNext()){
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
