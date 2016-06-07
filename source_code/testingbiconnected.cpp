#include "testingbiconnected.h"
#include "ui_testingbiconnected.h"

testingbiconnected::testingbiconnected(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testingbiconnected)
{
    ui->setupUi(this);
}

testingbiconnected::~testingbiconnected()
{
    delete ui;
}
