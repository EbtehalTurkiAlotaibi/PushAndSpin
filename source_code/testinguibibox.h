#ifndef TESTINGUIBIBOX_H
#define TESTINGUIBIBOX_H

#include <QDialog>

namespace Ui {
class testingUIBibox;
}

class testingUIBibox : public QDialog
{
    Q_OBJECT

public:
    explicit testingUIBibox(QWidget *parent = 0);
    ~testingUIBibox();
    int c0;
    int maxHandle;
    int handle_number;
    int free;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::testingUIBibox *ui;
};

#endif // TESTINGUIBIBOX_H
