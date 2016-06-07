#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int testing_plan_number;
    int solver;

private slots:

    void on_buttonBox_accepted();

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_comboBox_2_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
