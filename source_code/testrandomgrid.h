#ifndef TESTRANDOMGRID_H
#define TESTRANDOMGRID_H

#include <QDialog>

namespace Ui {
class testRandomGrid;
}

class testRandomGrid : public QDialog
{
    Q_OBJECT

public:
    explicit testRandomGrid(QWidget *parent = 0);
    ~testRandomGrid();
    int free_nodes;
    std::pair<int,int> HWRange;
    int stepSize;
    int HW;
    std::pair<int,int> freeRange;
    int plan;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_buttonBox_accepted();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

private:
    Ui::testRandomGrid *ui;
};

#endif // TESTRANDOMGRID_H
