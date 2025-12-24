#include "../include/helpWindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

HelpWindow::HelpWindow(QWidget* parent)
    : QDialog(parent)
{
    try {
        setWindowTitle("Memory State");
        
        QVBoxLayout* layout = new QVBoxLayout();

        QLabel* label = new QLabel(
    "Directories:\n"
    "    • .data: - to declare global data\n"
    "    • .threadInit: - to start the thread\n"
    "    • .threadEnd - to end the thread\n\n"
    "Commands:\n"
    "~Unary:\n"
    "for instance, INC x, where \n"
    "x - the name of the variable  \n"
    "    INC - to increase on 1 value\n"
    "    DEC - to decrease on 1 value\n"
    "    NEG - to change the sign\n"
    "    NOT - to use bitwise negation\n\n"
    "~Binary:\n"
    "for instance, MOV a, R1, where \n"
    "a - destination,\n"
    "R1 - source\n"
    "    MUL - to multiply\n"
    "    DIV - to divide\n"
    "    MOV - to transfer the data to another register/variable\n"
    "    ADD - to sum\n"
    "    SUB - to subtract\n"
    "    CMP - to compare\n"
    "    AND - logic and\n"
    "    OR - logic or\n"
    "    XOR - logic xor\n\n"
    "~Jump:\n"
    "    JMP - to jump on other address\n"
    "    JE - to jump on other address (dependence on flag ather CMP)\n\n"
    "~DataDeclaration: \n"
    "for instance, x DX y, where \n"
    "x - the name of the variable\n"
    "y - the value of the variable\n"
    "    tDB - declair 1 byte \n"
    "    DW - declair 2 byte\n"
    "    DD - declair 4 byte\n"
    "    DQ - declair 8 byte"
);
        label->setAlignment(Qt::AlignCenter);
        
        QPushButton* closeBtn = new QPushButton("Close");
        
        layout->addWidget(label);
        layout->addWidget(closeBtn);
        
        setLayout(layout);

        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
        
        setMinimumSize(400, 300);
        
        setModal(false);
        
    } catch (const std::exception& e) {
        qDebug() << "EXCEPTION in MemoryStateWindow constructor:" << e.what();
    } catch (...) {
        qDebug() << "UNKNOWN EXCEPTION in MemoryStateWindow constructor";
    }
    
    qDebug() << "=== MemoryStateWindow constructor FINISHED ===";
}