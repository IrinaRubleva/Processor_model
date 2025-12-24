#pragma once

#include <QDialog>
#include <QLabel>

class HelpWindow : public QDialog {
    Q_OBJECT
    
public:
    explicit HelpWindow(QWidget* parent = nullptr);
};