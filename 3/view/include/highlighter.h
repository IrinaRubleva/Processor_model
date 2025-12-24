#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression> 


class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT
    
public:
    explicit Highlighter(QTextDocument* parent = nullptr);
    
protected:
    void highlightBlock(const QString& text) override;
    
private:
    QTextCharFormat instructionFormat; 
    QTextCharFormat registerFormat;    
    QTextCharFormat labelFormat;       
    QTextCharFormat dataFormat;    
    QTextCharFormat numberFormat; 
    QTextCharFormat commentFormat;
};