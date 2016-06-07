#include "testinguibibox.h"
#include "ui_testinguibibox.h"

testingUIBibox::testingUIBibox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testingUIBibox)
{
    ui->setupUi(this);
}

testingUIBibox::~testingUIBibox()
{
    delete ui;
}

void testingUIBibox::on_buttonBox_accepted()
{
    free= ui->lineEdit->text().toInt();
    c0=ui->lineEdit_2->text().toInt();
    handle_number=ui->lineEdit_3->text().toInt();
    maxHandle=ui->lineEdit_4->text().toInt();
}

void testingUIBibox::on_buttonBox_rejected()
{
    this->close();
}
