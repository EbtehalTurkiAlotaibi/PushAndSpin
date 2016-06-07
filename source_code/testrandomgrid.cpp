#include "testrandomgrid.h"
#include "ui_testrandomgrid.h"

testRandomGrid::testRandomGrid(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testRandomGrid)
{
    ui->setupUi(this);
}

testRandomGrid::~testRandomGrid()
{
    delete ui;
}

void testRandomGrid::on_pushButton_clicked()
{
    free_nodes = ui->lineEdit->text().toInt();
    HWRange = std::make_pair(ui->lineEdit_9->text().toInt(),ui->lineEdit_10->text().toInt());
    stepSize = ui->lineEdit_8->text().toInt();
    for (int i=HWRange.first; i<HWRange.second; i+=stepSize) {
        QString str = "Node#";
        str.append(QString::number(i*i));
        str.append(",Free#");
        str.append(QString::number(free_nodes));
        str.append(",H=");
        str.append(QString::number(i));
        str.append(",W=");
        str.append(QString::number(i));
        ui->textBrowser->append(str);
    }
}

void testRandomGrid::on_pushButton_2_clicked()
{
    HW = ui->lineEdit_3->text().toInt();
    freeRange = std::make_pair(ui->lineEdit_11->text().toInt(),ui->lineEdit_12->text().toInt());
    stepSize = ui->lineEdit_13->text().toInt();
    for (int i=freeRange.first; i<freeRange.second; i+=stepSize) {
        QString str = "Node#";
        str.append(QString::number(i*i));
        str.append(",H=");
        str.append(QString::number(HW));
        str.append(",W=");
        str.append(QString::number(HW));
        str.append(",Free#");
        str.append(QString::number(i));
        ui->textBrowser_2->append(str);
    }


}

void testRandomGrid::on_buttonBox_accepted()
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

void testRandomGrid::on_radioButton_clicked()
{
    ui->radioButton_2->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);
    ui->lineEdit_11->setDisabled(true);
    ui->lineEdit_12->setDisabled(true);
    ui->lineEdit_13->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->textBrowser_2->setDisabled(true);

}

void testRandomGrid::on_radioButton_2_clicked()
{

    ui->radioButton->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->lineEdit_9->setDisabled(true);
    ui->lineEdit_10->setDisabled(true);
    ui->lineEdit_8->setDisabled(true);
    ui->pushButton->setDisabled(true);
    ui->textBrowser->setDisabled(true);
}
