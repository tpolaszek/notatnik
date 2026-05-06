#include "syntaxhighlighter.h"
#include <QJsonArray>
#include <QFont>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {}

// Ładowanid z pliku json zasad
void SyntaxHighlighter::loadFromJson(const QJsonObject& grammar){
    rules.clear(); // reset
    QJsonArray jsonRules = grammar["rules"].toArray();

    for (const QJsonValue& val : jsonRules){
        QJsonObject ruleObj = val.toObject();
        HighlightRule rule;

        //Użycie regexu jako sposób rozpoznawania znaczników
        rule.pattern = QRegularExpression(ruleObj["pattern"].toString());

        //Dodawanie formatowania
        QTextCharFormat fmt;
        fmt.setForeground(QColor(ruleObj["color"].toString())); // ustawia kolor czcionki
        if(ruleObj["bold"].toBool()) fmt.setFontWeight(QFont::Bold); // sprawdza czy w zasadach jest pogrubienie jeśli jest aplikuje je
        if(ruleObj["italic"].toBool()) fmt.setFontItalic(true); // sprawdza czy w zasadach jest pochylenie jeśli tak to je aplikuje

        rule.format = fmt;

        rules.append(rule);
    }
}

// Metoda podkreśla składnie
void SyntaxHighlighter::highlightBlock(const QString& text){
    // Dla każdej zasady podkreślenia ustawia formatowanie
    for(const HighlightRule & rule : rules){
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while(it.hasNext()){
            QRegularExpressionMatch match = it.next(); // sprawdza czy jest tekst który spełnia zasady wyrażenia regularnego
            setFormat(match.capturedStart(), match.capturedLength(), rule.format); // aplikuje formatowanie
        }
    }
}
