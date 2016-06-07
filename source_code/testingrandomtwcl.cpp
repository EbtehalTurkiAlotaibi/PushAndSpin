#include "testingrandomtwcl.h"
#include "ui_testingrandomtwcl.h"

testingRandomTWCL::testingRandomTWCL(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testingRandomTWCL)
{
    ui->setupUi(this);
}

testingRandomTWCL::~testingRandomTWCL()
{
    delete ui;
}

void testingRandomTWCL::on_radioButton_clicked()
{
    ui->radioButton_2->setDisabled(true);
    ui->lineEdit_4->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);
    ui->lineEdit_11->setDisabled(true);
    ui->lineEdit_12->setDisabled(true);
    ui->lineEdit_13->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->textBrowser_2->setDisabled(true);
}

void testingRandomTWCL::on_radioButton_2_clicked()
{
    ui->radioButton->setDisabled(true);
    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->lineEdit_9->setDisabled(true);
    ui->lineEdit_10->setDisabled(true);
    ui->lineEdit_8->setDisabled(true);
    ui->pushButton->setDisabled(true);
    ui->textBrowser->setDisabled(true);
}

void testingRandomTWCL::on_pushButton_clicked()
{

    node_number = ui->lineEdit_2->text().toInt();
    free_nodes = ui->lineEdit->text().toInt();
    blRange = std::make_pair(ui->lineEdit_9->text().toInt(),ui->lineEdit_10->text().toInt());
    stepSize = ui->lineEdit_8->text().toInt();
    for (int i=blRange.first; i<blRange.second; i+=stepSize) {
        QString str = "Node#";
        str.append(QString::number(node_number));
        str.append(",Free#");
        str.append(QString::number(free_nodes));
        str.append("BrigeLength=");
        str.append(QString::number(i));
        ui->textBrowser->append(str);
    }
}

void testingRandomTWCL::on_pushButton_2_clicked()
{
    node_number = ui->lineEdit_4->text().toInt();
    bridge_length = ui->lineEdit_3->text().toInt();
    freeRange = std::make_pair(ui->lineEdit_11->text().toInt(),ui->lineEdit_12->text().toInt());
    stepSize = ui->lineEdit_13->text().toInt();
    for (int i=freeRange.first; i<freeRange.second; i+=stepSize) {
        QString str = "Node#";
        str.append(QString::number(node_number));
        str.append(",BrigeLength=");
        str.append(QString::number(bridge_length));
        str.append(",Free#");
        str.append(QString::number(i));
        ui->textBrowser_2->append(str);
    }

}

void testingRandomTWCL::on_buttonBox_accepted()
{
    if (ui->radioButton->isEnabled())
        plan =1;
    else
        plan =2;

    this->close();
}
