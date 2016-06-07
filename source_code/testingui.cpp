#include "testingui.h"
#include "ui_testingui.h"
#include "qmessagebox.h"
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

TestingUI::TestingUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestingUI)
{
    ui->setupUi(this);
}

TestingUI::~TestingUI()
{
    delete ui;
}
void TestingUI::on_lineEdit_7_editingFinished()
{
    agent_number = ui->lineEdit_7->text().toInt();
         for (int i=0; i<agent_number; i++)
             ui->textBrowser_4->append(QString::number(i));
}

void TestingUI::on_buttonBox_accepted()
{
      std::cout<<"Done\n";
    node_number = ui->lineEdit_8->text().toInt();
    agent_number = ui->lineEdit_7->text().toInt();

    //Edges
    QStringList lines = ui->textEdit_15->toPlainText().split("\n"); // from
    QStringList lines_2 = ui->textEdit_16->toPlainText().split("\n"); // to

    if (lines.size()!=lines_2.size())
        QMessageBox::about(this,"Error","<From> nodes not equal <To> nodes !\n");

    else {

        for (int i=0; i<lines.size() && i<lines_2.size(); i++)
            Edges.push_back(std::make_pair(lines[i].toInt(),lines_2[i].toInt()));


        //Agents
        lines.clear();lines_2.clear();
        lines = ui->textEdit_13->toPlainText().split("\n");
        lines_2 = ui->textEdit_14->toPlainText().split("\n");

        if (lines.size()!=lines_2.size())
            QMessageBox::about(this,"Error","<Starts> nodes not equal <Goals>nodes !\n");

        else {
                for (int i=0; i<lines.size(); i++)
                    agents.push_back(Agent(i,lines[i].toInt(),lines_2[i].toInt()));

            this->close();
        }
    }


}


void TestingUI::on_buttonBox_rejected()
{
    this->close();
}
