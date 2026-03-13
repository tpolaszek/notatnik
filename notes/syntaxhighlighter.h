#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QJsonObject>

struct HighlightRule {
    QRegularExpression pattern;
    QTextCharFormat format;
};

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    SyntaxHighlighter(QTextDocument* parent = nullptr);
    void loadFromJson(const QJsonObject& grammar);

protected:
    void highlightBlock(const QString& text) override;

private:
    QVector<HighlightRule> rules;
};

#endif