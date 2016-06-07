#ifndef TESTINGRANDOMTWCL_H
#define TESTINGRANDOMTWCL_H

#include <QDialog>

namespace Ui {
class testingRandomTWCL;
}

class testingRandomTWCL : public QDialog
{
    Q_OBJECT

public:
    explicit testingRandomTWCL(QWidget *parent = 0);
    ~testingRandomTWCL();
    int node_number;
    int free_nodes;
    std::pair<int,int> blRange;
    int stepSize;
    int bridge_length;
    std::pair<int,int> freeRange;
    int plan;

private slots:
    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_buttonBox_accepted();

private:
    Ui::testingRandomTWCL *ui;
};

#endif // TESTINGRANDOMTWCL_H
