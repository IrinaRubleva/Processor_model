#pragma once

#include <QDialog>
#include <QLabel>
#include <QTextEdit>


class SimulatorPresenter;

class MemoryStateWindow : public QDialog {
    Q_OBJECT
    
public:
    explicit MemoryStateWindow(SimulatorPresenter* presenter, QWidget* parent = nullptr);
    
private:
    SimulatorPresenter* presenter;
};
