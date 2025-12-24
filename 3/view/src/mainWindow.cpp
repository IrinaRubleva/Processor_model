#include "../include/mainWindow.h"
#include "../include/glwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTableWidget>
#include <QFileDialog>      
#include <QMessageBox>      
#include <QFile>           
#include <QTextStream>  



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      presenter(std::make_unique<SimulatorPresenter>()),
      memoryWindow(nullptr), 
      helpWindow(nullptr),
      editorWindow(nullptr) 

{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);


    QHBoxLayout *topButtonsLayout = new QHBoxLayout();
    loadConfigBtn = new QPushButton("Load Config");
    loadAsmBtn = new QPushButton("Load ASM");
    runBtn = new QPushButton("RUN");
    helpBtn = new QPushButton("HELP");
    memState = new QPushButton("MEMORY STATE");
    editBtn = new QPushButton("Edit Code");

    
    topButtonsLayout->addWidget(loadConfigBtn);
    topButtonsLayout->addWidget(loadAsmBtn);
    topButtonsLayout->addWidget(runBtn);
    topButtonsLayout->addWidget(helpBtn);
    topButtonsLayout->addWidget(memState);
    topButtonsLayout->addWidget(editBtn);

 
    glWidget = new GLWidget(this);
    glWidget->setMinimumSize(640, 480);

    mainLayout->addLayout(topButtonsLayout);
    mainLayout->addWidget(glWidget, 1);



    connect(runBtn, &QPushButton::clicked,
            this, &MainWindow::runProcessor);
        
    connect(memState, &QPushButton::clicked,
            this, &MainWindow::showMemoryState);

    connect(loadConfigBtn, &QPushButton::clicked,
            this, &MainWindow::showLoadConfig);

    connect(loadAsmBtn, &QPushButton::clicked,
            this, &MainWindow::showAssembler);

    connect(helpBtn, &QPushButton::clicked,
            this,  &MainWindow::showHelp);

    connect(editBtn, &QPushButton::clicked,
            this, &MainWindow::showEditor);

    resize(1200, 800);
    setWindowTitle("CPU 3D Simulator");
}




void MainWindow::runProcessor() {
    presenter->run();
}



void MainWindow::showMemoryState(){
    if (memoryWindow) {
        memoryWindow->show();
        memoryWindow->raise();
        return;
    }

    memoryWindow = new MemoryStateWindow(presenter.get(), this);
    if (!memoryWindow) {
        return;
    }

    connect(memoryWindow, &MemoryStateWindow::finished, [this]() {
        memoryWindow->deleteLater();
        memoryWindow = nullptr;
    });
    
    memoryWindow->show();
    memoryWindow->raise();
}

void MainWindow::showLoadConfig(){
    QString filePath = QFileDialog::getOpenFileName(
        this, 
        "Выберите конфигурационный файл",
        "", 
        "JSON файлы (*.json)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }

    // if (presenter->getProcessor()) { 
    //     presenter->getProcessor()->reset();
    // }

    bool success = presenter->loadConfig(filePath.toStdString());
    
    if (success) {
        QMessageBox::information(this, "Успех", "Конфигурация загружена!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить конфигурацию :(");
    }
}


void MainWindow::showAssembler(){
    QString filePath = QFileDialog::getOpenFileName(
        this, 
        "Выберите файл с ассемблерным кодом",
        "", 
        "TXT файлы (*.txt)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    bool success  = presenter->loadProgram(filePath.toStdString());
    if (success) {
        QMessageBox::information(this, "Успех", "Код загружен!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить код :(");
    }
}


void MainWindow::showHelp(){
    if (helpWindow) {
        helpWindow->show();
        helpWindow->raise();
        return;
    }

    helpWindow = new HelpWindow(this);    
    if (!helpWindow) {
        return;
    }
    
    connect(helpWindow, &HelpWindow::finished, [this]() {
        helpWindow->deleteLater();
        helpWindow = nullptr;
    });
    
    helpWindow->show();
    helpWindow->raise();
}

void MainWindow::showEditor() {
    QString filePath = QFileDialog::getOpenFileName(
        this, 
        "Select ASM file to edit",
        "", 
        "ASM files (*.asm *.txt);;All files (*.*)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    openEditor(filePath);
}


void MainWindow::openEditor(const QString& filePath) {
    if (editorWindow) {
        editorWindow->close();
        editorWindow->deleteLater();
        editorWindow = nullptr;
    }

    editorWindow = new EditorWindow(filePath, this);
    connect(editorWindow, &EditorWindow::fileSaved, this, [this](const QString& savedFilePath) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Reload Program",
            "Program has been saved. Do you want to reload it?",
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply == QMessageBox::Yes) {
            presenter->resetProcessor();

            bool success = presenter->loadProgram(savedFilePath.toStdString());
            if (success) {
                QMessageBox::information(this, "Success", "Program reloaded!");
            } else {
                QMessageBox::warning(this, "Error", "Failed to reload program!");
            }
        }
    });
    connect(editorWindow, &EditorWindow::finished, [this]() {
        editorWindow->deleteLater();
        editorWindow = nullptr;
    });
    
    editorWindow->show();
    editorWindow->raise();
}



MainWindow::~MainWindow() {}