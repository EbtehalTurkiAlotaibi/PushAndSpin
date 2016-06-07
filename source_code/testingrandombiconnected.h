#ifndef TESTINGRANDOMBICONNECTED_H
#define TESTINGRANDOMBICONNECTED_H

#include <QDialog>

namespace Ui {
class testingRandomBiconnected;
}

class testingRandomBiconnected : public QDialog
{
    Q_OBJECT

public:
    explicit testingRandomBiconnected(QWidget *parent = 0);
    ~testingRandomBiconnected();
    int plan;
    int stepSize;
    int free;
    std::pair<int,int> c0Range;
    std::pair<int,int> handlesRange;
    std::pair<int,int> maxLenRange;
    int c0;
    int handles;
    int maxLen;
    std::pair<int,int> freeRange;


private slots:
    void on_radioButton_pressed();

    void on_buttonBox_accepted();

    void on_radioButton_2_pressed();

    void on_lineEdit_10_editingFinished();

    void on_pushButton_clicked();


    void on_pushButton_2_clicked();

private:
    Ui::testingRandomBiconnected *ui;
};

#endif // TESTINGRANDOMBICONNECTED_H
