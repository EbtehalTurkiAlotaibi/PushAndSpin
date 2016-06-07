#include "testuserinput.h"
#include "ui_testuserinput.h"
#include "QMessageBox"
#include "iostream"
using namespace std;

TestUserInput::TestUserInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestUserInput)
{
    ui->setupUi(this);
}

TestUserInput::~TestUserInput()
{
    delete ui;
}

void TestUserInput::on_buttonBox_accepted()
{
    node_number = ui->lineEdit->text().toInt();
    agent_number = ui->lineEdit_2->text().toInt();
            std::cout<<"Done\n";

          //Edges
          QStringList lines = ui->textEdit->toPlainText().split("\n"); // from
          QStringList lines_2 = ui->textEdit_2->toPlainText().split("\n"); // to

          if (lines.size()!=lines_2.size())
              QMessageBox::about(this,"Error","<From> nodes not equal <To> nodes !\n");

          else {

              for (int i=0; i<lines.size() && i<lines_2.size(); i++)
                  Edges.push_back(std::make_pair(lines[i].toInt(),lines_2[i].toInt()));


              //Agents
              lines.clear();lines_2.clear();
              lines = ui->textEdit_3->toPlainText().split("\n");
              lines_2 = ui->textEdit_4->toPlainText().split("\n");

              if (lines.size()!=lines_2.size())
                  QMessageBox::about(this,"Error","<Starts> nodes not equal <Goals>nodes !\n");

              else {
                      for (int i=0; i<lines.size(); i++)
                          agents.push_back(Agent(i,lines[i].toInt(),lines_2[i].toInt()));

                  this->close();
              }
          }
}

void TestUserInput::on_lineEdit_2_editingFinished()
{
    agent_number = ui->lineEdit_2->text().toInt();
    for (int i=0; i<agent_number; i++)
        ui->textBrowser->append(QString::number(i));
}

void TestUserInput::on_buttonBox_rejected()
{
    this->close();
}
