#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <memory>
#include "../../presenter/include/simulator_presenter.hpp"
#include "memStateWindow.h"
#include "helpWindow.h"
#include "editorWindow.h"

class GLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showMemoryState();
    void showLoadConfig();
    void showAssembler();
    void showHelp();
    void showEditor();
    void runProcessor();

private:
    std::unique_ptr<SimulatorPresenter> presenter;


    QPushButton *loadConfigBtn;
    QPushButton *loadAsmBtn;
    QPushButton *runBtn;
    QPushButton *helpBtn;
    QPushButton *editCommBtn;
    QPushButton *memState;

    GLWidget *glWidget;

    MemoryStateWindow *memoryWindow;
    HelpWindow *helpWindow;

    QPushButton *editBtn;
    EditorWindow *editorWindow;
    void openEditor(const QString& filePath);
};

#endif