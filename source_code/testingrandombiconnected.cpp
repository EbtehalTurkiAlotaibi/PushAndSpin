#include "testingrandombiconnected.h"
#include "ui_testingrandombiconnected.h"

testingRandomBiconnected::testingRandomBiconnected(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testingRandomBiconnected)
{
    ui->setupUi(this);
}

testingRandomBiconnected::~testingRandomBiconnected()
{
    delete ui;
}

void testingRandomBiconnected::on_radioButton_pressed()
{
    ui->radioButton_2->setDisabled(true);
    ui->lineEdit_11->setDisabled(true);
    ui->lineEdit_12->setDisabled(true);
    ui->lineEdit_13->setDisabled(true);
    ui->lineEdit_14->setDisabled(true);
    ui->lineEdit_15->setDisabled(true);
    ui->lineEdit_16->setDisabled(true);

}

void testingRandomBiconnected::on_buttonBox_accepted()
{
    if (ui->radioButton->isEnabled())
        plan=1;
    if (ui->radioButton_2->isEnabled())
        plan=2;

    if (plan==1) {
        free = ui->lineEdit->text().toInt();
        c0Range = std::make_pair(ui->lineEdit_2->text().toInt(),ui->lineEdit_3->text().toInt());
        handlesRange=  std::make_pair(ui->lineEdit_7->text().toInt(),ui->lineEdit_5->text().toInt());
        maxLenRange =std::make_pair(ui->lineEdit_9->text().toInt(),ui->lineEdit_10->text().toInt());
        stepSize = ui->lineEdit_8->text().toInt();
    }
    else {
    c0 = ui->lineEdit_13->text().toInt();
    handles = ui->lineEdit_12->text().toInt();
    maxLen = ui->lineEdit_11->text().toInt();
    freeRange = std::make_pair (ui->lineEdit_15->text().toInt(),ui->lineEdit_14->text().toInt());
    stepSize = ui->lineEdit_16->text().toInt();
    }
}

void testingRandomBiconnected::on_radioButton_2_pressed()
{
    ui->radioButton->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);
    ui->lineEdit_5->setDisabled(true);
    ui->lineEdit_7->setDisabled(true);
    ui->lineEdit_8->setDisabled(true);
    ui->lineEdit_9->setDisabled(true);
    ui->lineEdit_10->setDisabled(true);
}

void testingRandomBiconnected::on_lineEdit_10_editingFinished()
{


}



void testingRandomBiconnected::on_pushButton_clicked()
{
    free = ui->lineEdit->text().toInt();
    c0Range = std::make_pair(ui->lineEdit_2->text().toInt(),ui->lineEdit_3->text().toInt());
    handlesRange=  std::make_pair(ui->lineEdit_7->text().toInt(),ui->lineEdit_5->text().toInt());
    maxLenRange =std::make_pair(ui->lineEdit_9->text().toInt(),ui->lineEdit_10->text().toInt());
    stepSize = ui->lineEdit_8->text().toInt();

    for (int i=0; i<(c0Range.second-c0Range.first); i+=stepSize) {
        QString str = "Free=";
        str.append(QString::number(free));
        str.append(",C0=");
        str.append(QString::number(c0Range.first+i));
        str.append(",#handles=");
        str.append(QString::number(handlesRange.first+i));
        str.append(",maxLen=");
        str.append(QString::number(maxLenRange.first+i));
        ui->textBrowser->append(str);
    }
}


void testingRandomBiconnected::on_pushButton_2_clicked()
{
    c0 = ui->lineEdit_13->text().toInt();
    handles = ui->lineEdit_12->text().toInt();
    maxLen = ui->lineEdit_11->text().toInt();
    freeRange = std::make_pair (ui->lineEdit_15->text().toInt(),ui->lineEdit_14->text().toInt());
    stepSize = ui->lineEdit_16->text().toInt();

    for (int i=0; i<(freeRange.second-freeRange.first); i+=stepSize) {
        QString str = "Free=";
        str.append(QString::number(freeRange.first+i));
        str.append(",C0=");
        str.append(QString::number(c0));
        str.append(",#handles=");
        str.append(QString::number(handles));
        str.append(",maxLen=");
        str.append(QString::number(maxLen));
        ui->textBrowser_2->append(str);
    }
}

