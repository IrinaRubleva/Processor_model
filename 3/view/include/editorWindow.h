#pragma once

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>

class EditorWindow : public QDialog {
    Q_OBJECT
    
public:
    explicit EditorWindow(const QString& filePath, QWidget* parent = nullptr);
    ~EditorWindow();
    
    bool isModified() const { return textModified; }
    QString getFilePath() const { return filePath; }
    
signals:
    void fileSaved(const QString& filePath);
    
protected:
    void closeEvent(QCloseEvent* event) override;
    
private slots:
    void saveFile();
    void textChanged();
    
private:
    void loadFile();
    void updateTitle();
    bool saveChangesDialog();
    
    QString filePath;
    QTextEdit* textEdit;
    QPushButton* saveButton;
    QPushButton* cancelButton;
    bool textModified;
    QString originalContent;
};