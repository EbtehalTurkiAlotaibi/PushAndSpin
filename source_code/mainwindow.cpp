#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_buttonBox_accepted()
{
    std::cout<<"plan ="<<testing_plan_number<<"\n";
    testing_plan_number=ui->comboBox_2->currentIndex();
    solver=ui->comboBox->currentIndex();

}

void MainWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{

    ui->comboBox->clear();
    QStringList str;
    str.append("Select ...");
    str.append("Push And Swap");
    str.append("Push And Spin");

    if (ui->comboBox_2->currentIndex()==4 || ui->comboBox_2->currentIndex()==1 ||ui->comboBox_2->currentIndex()==6)
        str.append("Bibox");

    ui->comboBox->addItems(str);
}

void MainWindow::on_comboBox_2_activated(const QString &arg1)
{

}
