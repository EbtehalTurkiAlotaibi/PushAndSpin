#ifndef TESTINGRANDOMGRID_H
#define TESTINGRANDOMGRID_H

#include <QDialog>

namespace Ui {
class testingRandomGrid;
}

class testingRandomGrid : public QDialog
{
    Q_OBJECT

public:
    explicit testingRandomGrid(QWidget *parent = 0);
    ~testingRandomGrid();
    int node_number;
    int free_nodes;
    std::pair<int,int> HWRange;
    int stepSize;
    int HW;
    std::pair<int,int> freeRange;
    int plan;

private:
    Ui::testingRandomGrid *ui;

private slots:
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_buttonBox_accepted();

};

#endif // TESTINGRANDOMGRID_H
