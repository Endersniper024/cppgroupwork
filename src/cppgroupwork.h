#pragma once
#include "ui_cppgroupwork.h"
#include <QMainWindow>

class cppgroupwork : public QMainWindow {
    Q_OBJECT
    
public:
    cppgroupwork(QWidget* parent = nullptr);
    ~cppgroupwork();

private:
    Ui_cppgroupwork* ui;
};