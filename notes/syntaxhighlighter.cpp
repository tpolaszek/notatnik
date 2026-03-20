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
        fmt.setForeground(QColor(ruleObj["color"].toString()));
        if(ruleObj["bold"].toBool()) fmt.setFontWeight(QFont::Bold);
        if(ruleObj["italic"].toBool()) fmt.setFontItalic(true);

        rule.format = fmt;

        rules.append(rule);
    }
}

void SyntaxHighlighter::highlightBlock(const QString& text){
    for(const HighlightRule & rule : rules){
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while(it.hasNext()){
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}