#include "cppgroupwork.h"

cppgroupwork::cppgroupwork(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_cppgroupwork)
{
    ui->setupUi(this);
}

cppgroupwork::~cppgroupwork()
{
    delete ui; 
}