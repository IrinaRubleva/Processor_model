#include "../include/memStateWindow.h"
#include "../../presenter/include/simulator_presenter.hpp"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QTextEdit>



MemoryStateWindow::MemoryStateWindow(SimulatorPresenter* presenter, QWidget* parent)
    : QDialog(parent), presenter(presenter) 
    {
    try {
        setWindowTitle("Memory State");
        
        QVBoxLayout* layout = new QVBoxLayout();
        
        QLabel* label = new QLabel("Memory State");
        label->setAlignment(Qt::AlignCenter);
        

        QTextEdit* textEdit = new QTextEdit();
        textEdit->setReadOnly(true);
        
        if (presenter) {
            std::string state = presenter->showMemoryState();
            QString memoryState = QString::fromStdString(state);
            textEdit->setText(memoryState);
        } else {
            textEdit->setText("ViewModel not available");
        }


        QPushButton* closeBtn = new QPushButton("Close");
        
        layout->addWidget(label);
        layout->addWidget(textEdit);
        layout->addWidget(closeBtn);
        
        setLayout(layout);

        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
        
        setMinimumSize(600, 500);
        
        setModal(false);
        
    } catch (const std::exception& e) {
        qDebug() << "EXCEPTION in MemoryStateWindow constructor:" << e.what();
    } catch (...) {
        qDebug() << "UNKNOWN EXCEPTION in MemoryStateWindow constructor";
    }
    
    qDebug() << "=== MemoryStateWindow constructor FINISHED ===";
}





