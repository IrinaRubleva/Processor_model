#include "../include/editorWindow.h"
#include "../include/highlighter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <QDebug>
#include <QFont>
#include <QFileInfo>
#include <QMessageBox>
#include <QShortcut>


EditorWindow::EditorWindow(const QString& filePath, QWidget* parent)
    : QDialog(parent), filePath(filePath), textModified(false)
{
    setWindowTitle("Code Editor - " + QFileInfo(filePath).fileName());
    setMinimumSize(800, 600);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    
    textEdit = new QTextEdit(this);
    textEdit->setFont(QFont("Consolas", 11));


    QFontMetrics metrics(textEdit->font());
    int tabWidth = 4 * metrics.horizontalAdvance(' ');
    textEdit->setTabStopDistance(tabWidth);


    mainLayout->addWidget(textEdit);
    

    // Сохранить (Ctrl+S)
    QShortcut* saveShortcut = new QShortcut(QKeySequence::Save, this);
    connect(saveShortcut, &QShortcut::activated, this, &EditorWindow::saveFile);
    
    // Закрыть (Ctrl+W)
    QShortcut* closeShortcut = new QShortcut(QKeySequence::Close, this);
    connect(closeShortcut, &QShortcut::activated, this, &EditorWindow::close);
    
    // Отменить (Ctrl+Z)
    QShortcut* undoShortcut = new QShortcut(QKeySequence::Undo, textEdit);
    connect(undoShortcut, &QShortcut::activated, textEdit, &QTextEdit::undo);
    
    // Повторить (Ctrl+Y)
    QShortcut* redoShortcut = new QShortcut(QKeySequence::Redo, textEdit);
    connect(redoShortcut, &QShortcut::activated, textEdit, &QTextEdit::redo);

    new Highlighter(textEdit->document());

    textEdit->setFont(QFont("Consolas", 13));  
    textEdit->setStyleSheet("QTextEdit {"
                        "background-color: #1e1e1e;"
                       "color: #ffffff;"  
                       "border: 1px solid #3c3c3c;"
                       "selection-background-color: #264f78;"
                       "}");


    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    saveButton = new QPushButton("Save", this);
    cancelButton = new QPushButton("Cancel", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    

    connect(textEdit, &QTextEdit::textChanged, this, &EditorWindow::textChanged);
    connect(saveButton, &QPushButton::clicked, this, &EditorWindow::saveFile);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
   
    loadFile();
    
    
    originalContent = textEdit->toPlainText();
    textModified = false;
    updateTitle();
}

EditorWindow::~EditorWindow() {
    qDebug() << "EditorWindow destroyed";
}

void EditorWindow::loadFile() {
    QFile file(filePath);
    if (!file.exists()) {
        textEdit->setPlainText("# File not found\n# Creating new file");
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        textEdit->setPlainText("# Error: Cannot open file for reading");
        return;
    }
    
    QTextStream stream(&file);
    textEdit->setPlainText(stream.readAll());
    file.close();
}

void EditorWindow::saveFile() {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", 
            "Cannot save file: " + file.errorString());
        return;
    }
    
    QTextStream stream(&file);
    stream << textEdit->toPlainText();
    file.close();
    
    textModified = false;
    originalContent = textEdit->toPlainText();
    updateTitle();
    
    emit fileSaved(filePath);
    
    QMessageBox::information(this, "Success", 
        "File saved successfully!");
}

void EditorWindow::textChanged() {
    QString currentContent = textEdit->toPlainText();
    textModified = (currentContent != originalContent);
    updateTitle();
}

void EditorWindow::updateTitle() {
    QString title = "Code Editor - " + QFileInfo(filePath).fileName();
    if (textModified) {
        title += " *";
    }
    setWindowTitle(title);
}

void EditorWindow::closeEvent(QCloseEvent* event) {
    if (textModified) {
        if (!saveChangesDialog()) {
            event->ignore();  
            return;
        }
    }
    event->accept();
}

bool EditorWindow::saveChangesDialog() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Unsaved Changes");
    msgBox.setText("The document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    
    int result = msgBox.exec();
    
    switch (result) {
    case QMessageBox::Save:
        saveFile();
        return true;
    case QMessageBox::Cancel:
    default:
        return false;
    }
}