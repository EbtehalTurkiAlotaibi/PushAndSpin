#ifndef TESTINGCRITICALPROBLEMS_H
#define TESTINGCRITICALPROBLEMS_H

#include <QDialog>

namespace Ui {
class TestingCriticalProblems;
}

class TestingCriticalProblems : public QDialog
{
    Q_OBJECT

public:
    explicit TestingCriticalProblems(QWidget *parent = 0);
    ~TestingCriticalProblems();
    int problem_index;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::TestingCriticalProblems *ui;
};

#endif // TESTINGCRITICALPROBLEMS_H
