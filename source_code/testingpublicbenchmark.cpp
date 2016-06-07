#include "testingpublicbenchmark.h"
#include "ui_testingpublicbenchmark.h"

TestingPublicBenchmark::TestingPublicBenchmark(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestingPublicBenchmark)
{
    ui->setupUi(this);
}

TestingPublicBenchmark::~TestingPublicBenchmark()
{
    delete ui;
}

void TestingPublicBenchmark::on_buttonBox_accepted()
{
    mapName = ui->comboBox->currentText().toStdString();
    agentRange = std::make_pair(ui->lineEdit->text().toInt(),ui->lineEdit_2->text().toInt()+1) ;
    stepSize = ui->lineEdit_3->text().toInt();



}

void TestingPublicBenchmark::on_lineEdit_3_editingFinished()
{
    mapName = ui->comboBox->currentText().toStdString();
    QString str = ui->comboBox->currentText();
    agentRange = std::make_pair(ui->lineEdit->text().toInt(),ui->lineEdit_2->text().toInt()) ;
    stepSize = ui->lineEdit_3->text().toInt();
    for (int i=agentRange.first; i<agentRange.second; i+=stepSize) {
        QString row = "Map=";
        row.append(str);
        row.append(", Agents=");
        row.append(QString::number(i));
        ui->textBrowser->append(row);
    }
}

void TestingPublicBenchmark::on_lineEdit_3_textEdited(const QString &arg1)
{

}

void TestingPublicBenchmark::on_lineEdit_3_returnPressed()
{

}
