#include "testingrandomgrid.h"
#include "ui_testingrandomgrid.h"

testingRandomGrid::testingRandomGrid(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testingRandomGrid)
{
    ui->setupUi(this);
}

testingRandomGrid::~testingRandomGrid()
{
    delete ui;
}


void testingRandomGrid::on_radioButton_clicked()
{
    ui->radioButton_2->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);
    ui->lineEdit_11->setDisabled(true);
    ui->lineEdit_12->setDisabled(true);
    ui->lineEdit_13->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->textBrowser_2->setDisabled(true);
}

void testingRandomGrid::on_radioButton_2_clicked()
{
    ui->radioButton->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->lineEdit_9->setDisabled(true);
    ui->lineEdit_10->setDisabled(true);
    ui->lineEdit_8->setDisabled(true);
    ui->pushButton->setDisabled(true);
    ui->textBrowser->setDisabled(true);
}

void testingRandomGrid::on_pushButton_clicked()
{

    free_nodes = ui->lineEdit->text().toInt();
    HWRange = std::make_pair(ui->lineEdit_9->text().toInt(),ui->lineEdit_10->text().toInt());
    stepSize = ui->lineEdit_8->text().toInt();
    for (int i=HWRange.first; i<HWRange.second; i+=stepSize) {
        QString str = "Node#";
        str.append(QString::number(node_number));
        str.append(",Free#");
        str.append(QString::number(free_nodes));
        str.append(",H=");
        str.append(QString::number(i));
        str.append(",W=");
        str.append(QString::number(i));
        ui->textBrowser->append(str);
    }
}

void testingRandomGrid::on_pushButton_2_clicked()
{
    HW = ui->lineEdit_3->text().toInt();
    freeRange = std::make_pair(ui->lineEdit_11->text().toInt(),ui->lineEdit_12->text().toInt());
    stepSize = ui->lineEdit_13->text().toInt();
    for (int i=freeRange.first; i<freeRange.second; i+=stepSize) {
        QString str = "Node#";
        str.append(QString::number(node_number));
        str.append(",H=");
        str.append(QString::number(HW));
        str.append(",W=");
        str.append(QString::number(HW));
        str.append(",Free#");
        str.append(QString::number(i));
        ui->textBrowser_2->append(str);
    }

}

void testingRandomGrid::on_buttonBox_accepted()
{
    if (ui->radioButton->isEnabled())
        plan =1;
    else
        plan =2;

    if (plan==1) {
        free_nodes = ui->lineEdit->text().toInt();
        HWRange = std::make_pair(ui->lineEdit_9->text().toInt(),ui->lineEdit_10->text().toInt());
        stepSize = ui->lineEdit_8->text().toInt();
    }
    if (plan==2) {
        HW = ui->lineEdit_3->text().toInt();
        freeRange = std::make_pair(ui->lineEdit_11->text().toInt(),ui->lineEdit_12->text().toInt());
        stepSize = ui->lineEdit_13->text().toInt();
    }

    this->close();
}
