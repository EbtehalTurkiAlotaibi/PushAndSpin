#ifndef TESTINGBICONNECTED_H
#define TESTINGBICONNECTED_H

#include <QDialog>

namespace Ui {
class testingbiconnected;
}

class testingbiconnected : public QDialog
{
    Q_OBJECT

public:
    explicit testingbiconnected(QWidget *parent = 0);
    ~testingbiconnected();

private:
    Ui::testingbiconnected *ui;
};

#endif // TESTINGBICONNECTED_H
