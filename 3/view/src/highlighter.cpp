#include "../include/highlighter.h"
#include <QColor>

Highlighter::Highlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    instructionFormat.setForeground(QColor("#4ec9b0")); 
    instructionFormat.setFontWeight(QFont::Bold);
    
    registerFormat.setForeground(QColor("#cfa7cb")); 
    
   labelFormat.setForeground(QColor("#aebf3b"));
    labelFormat.setFontWeight(QFont::Bold);
    
    dataFormat.setForeground(QColor("#608b4e"));  
    dataFormat.setFontWeight(QFont::Bold);
    
    numberFormat.setForeground(QColor("#ce9178")); 
    
    commentFormat.setForeground(QColor("#6a9955")); 
    commentFormat.setFontItalic(true);
}

void Highlighter::highlightBlock(const QString& text) {
    
    QRegularExpression commentPattern(R"((;|#)[^\n]*)");
    QRegularExpressionMatchIterator commentMatch = commentPattern.globalMatch(text);
    
    while (commentMatch.hasNext()) {
        QRegularExpressionMatch match = commentMatch.next();
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
    }
    
    QRegularExpression numberPattern(R"(\b\d+\b)");
    QRegularExpressionMatchIterator numberMatch = numberPattern.globalMatch(text);
    
    while (numberMatch.hasNext()) {
        QRegularExpressionMatch match = numberMatch.next();
        setFormat(match.capturedStart(), match.capturedLength(), numberFormat);
    }
    
    QRegularExpression labelPattern(R"(^\s*[A-Za-z_][A-Za-z0-9_]*:)");
    QRegularExpressionMatchIterator labelMatch = labelPattern.globalMatch(text);
    
    while (labelMatch.hasNext()) {
        QRegularExpressionMatch match = labelMatch.next();
        setFormat(match.capturedStart(), match.capturedLength(), labelFormat);
    }
    
    QStringList instructions = {
        "INC", "DEC", "NEG", "NOT",
        "MOV", "ADD", "SUB", "MUL", "DIV", "CMP", "AND", "OR", "XOR",
        "JMP", "JE", "JNE", "JG", "JL", "JGE", "JLE",
        "THRI", "THRE"
    };
    
    for (const QString& instr : instructions) {
        QRegularExpression instrPattern("\\b" + instr + "\\b");
        QRegularExpressionMatchIterator instrMatch = instrPattern.globalMatch(text);
        
        while (instrMatch.hasNext()) {
            QRegularExpressionMatch match = instrMatch.next();
            setFormat(match.capturedStart(), match.capturedLength(), instructionFormat);
        }
    }
    
    QRegularExpression registerPattern(R"(\bR\d{1,2}\b)");
    QRegularExpressionMatchIterator registerMatch = registerPattern.globalMatch(text);
    
    while (registerMatch.hasNext()) {
        QRegularExpressionMatch match = registerMatch.next();
        setFormat(match.capturedStart(), match.capturedLength(), registerFormat);
    }
    
    QStringList dataDirectives = {"DB", "DW", "DD", "DQ"};
    
    for (const QString& data : dataDirectives) {
        QRegularExpression dirPattern("\\b" + data + "\\b");
        QRegularExpressionMatchIterator dirMatch = dirPattern.globalMatch(text);
        
        while (dirMatch.hasNext()) {
            QRegularExpressionMatch match = dirMatch.next();
            setFormat(match.capturedStart(), match.capturedLength(), dataFormat);
        }
    }
}