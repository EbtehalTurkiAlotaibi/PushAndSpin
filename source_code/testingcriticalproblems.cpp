#include "testingcriticalproblems.h"
#include "ui_testingcriticalproblems.h"

TestingCriticalProblems::TestingCriticalProblems(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestingCriticalProblems)
{
    ui->setupUi(this);
}

TestingCriticalProblems::~TestingCriticalProblems()
{
    delete ui;
}

void TestingCriticalProblems::on_buttonBox_accepted()
{
    problem_index = ui->comboBox->currentIndex();
}

void TestingCriticalProblems::on_buttonBox_rejected()
{
    this->close();
}
